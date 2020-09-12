//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "QuicConnectionClient.h"

#define Min_FlowControl_Window 16 * 1024  // 16 KB
#define Init_Connection_FlowControl_Window 64 * 1024  // 16 KB

#define Max_Stream_ReceiveWindow 16 * 1024 * 1024   // 16 MB
#define Max_Connection_ReceiveWindow 24 * 1024 * 1024  // 24 MB
#define DEAFULT_STREAM_NUM 10
#define MAX_PAYLOAD_PACKET 2000
#define ACKTHRESH 3
#define SSTHRESH_CHANGE_THIS 10
#define kPacketThreshold 3
#define QUIC_SHORT_HEADER_LENGTH 16
namespace inet {

QuicConnectionClient::QuicConnectionClient() {
    // TODO Auto-generated constructor stub

}

QuicConnectionClient::QuicConnectionClient(int* connection_data, int connection_data_size,L3Address destination) {

  // QuicConnection();//need to check if its ok

    stream_arr = new QuicStreamArr(connection_data_size);
    this->congestion_alg=new QuicNewReno();

    for (int i=0; i<connection_data_size; i++) {
        AddNewStream(connection_data[i],i);
    }

    this->packet_counter = 0;
    this->send_una = 0;
    this->last_rcvd_ACK = 0;
    this->dup_ACKS = 0;
    this->destination=destination;
    send_not_ACKED_queue = new std::list<Packet*>();
    waiting_retransmission = new std::list<Packet*>();
    packets_to_send = new std::list<Packet*>();
    fsm.setState(QUIC_S_CLIENT_INITIATE_HANDSHAKE);
    event->setKind(QUIC_E_CLIENT_INITIATE_HANDSHAKE);
}


QuicConnectionClient::~QuicConnectionClient() {
    // TODO Auto-generated destructor stub
}


Packet* QuicConnectionClient::createQuicDataPacket(StreamsData* streams_data) {
    char msgName[32];
    sprintf(msgName, "QUIC packet number-%d", packet_counter);

    Packet *packet_to_send = new Packet(msgName);
    auto QuicHeader = makeShared<QuicPacketHeader>();
    QuicHeader->setPacket_number(packet_counter);
    QuicHeader->setSrc_connectionID(connection_source_ID);
    QuicHeader->setDest_connectionID(connection_dest_ID);
    QuicHeader->setPacket_type(2);
    QuicHeader->setChunkLength(B(sizeof(int)*4));
    packet_to_send->insertAtFront(QuicHeader);

    const auto &payload = makeShared<QuicData>();
    int msgByteLength = streams_data->getTotalSize();
    payload->setChunkLength(B(msgByteLength));
    payload->setStream_frames(streams_data);
    packet_to_send->insertAtBack(payload);
    packet_counter++;
    return packet_to_send;
}


StreamsData* QuicConnectionClient::CreateSendData(int max_payload) {
    StreamsData *data_to_send = this->stream_arr->DataToSend(max_payload-QUIC_SHORT_HEADER_LENGTH);
    int bytes_sent = data_to_send->getTotalSize();
    congestion_alg->SetSndMax(bytes_sent);
    return data_to_send;
}

void QuicConnectionClient::updateMaxStreamData(int stream_id, int max_stream_data_offset){
    stream* curr_stream = stream_arr->getStream(stream_id);
    curr_stream->flow_control_recieve_offset=max_stream_data_offset;
    curr_stream->flow_control_recieve_window=curr_stream->flow_control_recieve_offset-curr_stream->highest_recieved_byte_offset;
}


void QuicConnectionClient::AddNewStream(int stream_size, int index) {
    this->stream_arr->AddNewStream(stream_size, index);
}


Packet* QuicConnectionClient::ProcessInitiateHandshake(Packet* packet) {
    EV << " ProcessInitiateHandshake in quicConnection" << endl;
    char msgName[32];
    sprintf(msgName, "QUIC INITIAL HANDSHAKE");
    int src_ID = 0;
    int dest_ID = std::rand();
    Packet *packet_to_send = new Packet(msgName);
    auto QuicHeader = makeShared<QuicPacketHeader>();
    // need to set header's dest & source connection ID & setChunkLength &
    QuicHeader->setPacket_number(0);
    QuicHeader->setSrc_connectionID(src_ID);
    QuicHeader->setDest_connectionID(dest_ID);
    QuicHeader->setPacket_type(0);
    QuicHeader->setChunkLength(B(sizeof(int)*4));
    packet_to_send->insertAtFront(QuicHeader);
    this->SetSourceID(src_ID);
    this->SetDestID(dest_ID);

    const auto &payload = makeShared<QuicHandShakeData>();
    payload->setInitial_source_connection_id(src_ID);
    payload->setChunkLength(B(sizeof(int)*1));
    packet_to_send->insertAtBack(payload);;
    return packet_to_send;
}


void QuicConnectionClient::ProcessClientHandshakeResponse(Packet* packet) {
    auto header = packet->popAtFront<QuicPacketHeader>();
    auto data = packet->peekData<QuicHandShakeData>(); // get data from packet
    // process data
    int initial_stream_window = data->getInitial_max_stream_data();
    int initial_connection_window = data->getInitial_max_data();
    int max_payload_ = data->getMax_udp_payload_size();
    this->max_payload = max_payload_;
    this->connection_max_flow_control_window_size = initial_connection_window;
    this->connection_flow_control_recieve_offset = initial_connection_window;
    this->connection_flow_control_recieve_window = initial_connection_window;
    int sum_stream_window_size = this->stream_arr->getSumStreamsWindowSize();
    this->stream_arr->setAllStreamsWindows(initial_stream_window);
    this->congestion_alg->SetSndMss(max_payload);
    this->congestion_alg->SetSndCwnd(max_payload*2);//initial congestion window
    this->congestion_alg->SetSsThresh(max_payload*SSTHRESH_CHANGE_THIS);// initial slow start threshold.
    this->congestion_alg->SetSndWnd(std::min(connection_max_flow_control_window_size,sum_stream_window_size));
}

void QuicConnectionClient::ProcessClientACK(Packet* ack_packet){
    auto header = ack_packet->popAtFront<QuicPacketHeader>();
    auto ACK_data = ack_packet->peekData<QuicACKFrame>();
    // uint32_t old_send_una;
    // go over all the ACKED packets in this ACK frame
    int num_of_ACKED_packets = ACK_data->getFirst_ACK_range();
    int largest = ACK_data->getLargest_acknowledged();
    int smallest = largest - num_of_ACKED_packets + 1;
    for (int i = 0; i < num_of_ACKED_packets; i++) {
        int current_packet = smallest + i;
        Packet* copy_of_ACKED_packet = RemovePacketFromQueue(current_packet);
        updateFlowControl(copy_of_ACKED_packet); //update flow control
        simtime_t acked_time = copy_of_ACKED_packet->getTimestamp();
        UpdateRtt(acked_time); //update rtt measurement
        auto data = copy_of_ACKED_packet->peekData<QuicData>(); // get data from packet
        const StreamsData* streams_data = data->getStream_frames();
        uint32_t size_in_bytes = streams_data->getTotalSize();
        // update congestion control
        old_send_una = send_una;
        send_una += size_in_bytes;
        congestion_alg->SetSndUnA(send_una);
        int sum_stream_window_size = this->stream_arr->getSumStreamsWindowSize();
        congestion_alg->SetSndWnd(std::min(connection_max_flow_control_window_size,sum_stream_window_size));
    }
}

void QuicConnectionClient::ProcessClientSend(){
    EV << "congestion window size: " << congestion_alg->GetSndCwnd() << endl;

    int actual_window = std::min(connection_flow_control_recieve_window,congestion_alg->GetSndCwnd());
    int curr_payload_size = max_payload;

    // trying to send retransmissions first
    std::list<Packet*>::iterator it = waiting_retransmission->begin();
    while (actual_window != 0 && it != waiting_retransmission->end()) {
        if ((*it)->getByteLength() > actual_window) {
            it++;
            continue;
        }
        Packet* packet_to_send = *it;
        it++;
        waiting_retransmission->remove(packet_to_send);
        packets_to_send->push_back(packet_to_send);
        int packet_size = packet_to_send->getByteLength();
        actual_window -= packet_size;
        // create copy
        Packet* copy_packet_to_send = packet_to_send->dup();
        copy_packet_to_send->setTimestamp(simTime());
        this->send_not_ACKED_queue->push_back(copy_packet_to_send);
    }

    // create new data packets
    while (actual_window != 0) {
        if (actual_window < max_payload)
            curr_payload_size = actual_window;

        StreamsData *send_data = this->CreateSendData(curr_payload_size);
        Packet* packet_to_send = createQuicDataPacket(send_data);
        packets_to_send->push_back(packet_to_send);
        int packet_size = packet_to_send->getByteLength();
        actual_window -= packet_size;
        // create copy
        Packet* copy_packet_to_send = packet_to_send->dup();
        copy_packet_to_send->setTimestamp(simTime());
        this->send_not_ACKED_queue->push_back(copy_packet_to_send);
    }
}


Packet* QuicConnectionClient::RemovePacketFromQueue(int packet_number){
    Packet* packet_to_remove;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number()==packet_number){
                packet_to_remove=*it;
                send_not_ACKED_queue->remove(packet_to_remove);
                break;
            }
    }
    return packet_to_remove;
}

Packet* QuicConnectionClient::findPacket(int packet_number) {
    Packet* packet_to_peek = NULL;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number()==packet_number){
                packet_to_peek=*it;
                break;
            }
    }
    return packet_to_peek;
}

std::list<Packet*>* QuicConnectionClient::getLostPackets(int largest) {
    std::list<Packet*>* lost_packets = new std::list<Packet*>();
    int packet_threshold = largest - kPacketThreshold; // all packets under packet_threshold are lost

    std::list<Packet*>::iterator it = send_not_ACKED_queue->begin();
    while (it != send_not_ACKED_queue->end()) {
        auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
        Packet* packet_to_remove;
        if (current_header->getPacket_number() <= packet_threshold){
            int packet_gap = largest-current_header->getPacket_number();
            congestion_alg->receivedDuplicateAck(packet_gap);
            packet_to_remove=*it;
            it++;
            createRetransmitPacket(packet_to_remove);
            lost_packets->push_back(packet_to_remove);
        }
        else {
            it++;
        }
    }

    if (lost_packets->empty())
        congestion_alg->receivedDataAck(old_send_una);
    return lost_packets;
}

std::list<Packet*>* QuicConnectionClient::getPacketsToSend() {
    return packets_to_send;
}


simtime_t QuicConnectionClient::GetRto(){
    return congestion_alg->GetRto();
}

void QuicConnectionClient::updateFlowControl(Packet* acked_packet){
    auto header = acked_packet->popAtFront<QuicPacketHeader>();
    auto data = acked_packet->peekData<QuicData>();
    // get frames from packet
    const StreamsData* streams_data = data->getStream_frames();
    std::vector<stream_frame*> frames_in_packet = streams_data->getFramesArray();
    // update per stream
    for (std::vector<stream_frame*>::iterator it =
            frames_in_packet.begin(); it != frames_in_packet.end(); ++it) {
            stream_frame* curr_frame = *it;
            int stream_id = curr_frame->stream_id;
            int offset = curr_frame->offset;
            int length = curr_frame->length;
            stream* curr_stream = stream_arr->getStream(stream_id);
            curr_stream->highest_recieved_byte_offset = offset + length;
            curr_stream->flow_control_recieve_window=curr_stream->flow_control_recieve_offset-curr_stream->highest_recieved_byte_offset;
    }

    //update connection flow control
    int sum_highest_offsets = this->GetMaxOffset();
    connection_flow_control_recieve_window = connection_flow_control_recieve_offset -sum_highest_offsets;

}

void QuicConnectionClient::UpdateRtt(simtime_t acked_time) {
    uint32 acked_time_int = this->congestion_alg->convertSimtimeToTS(acked_time);
    this->congestion_alg->rttMeasurementCompleteUsingTS(acked_time_int);
}

Packet* QuicConnectionClient::updatePacketNumber(Packet* old_packet) {
    auto current_header=old_packet->popAtFront<QuicPacketHeader>();
    auto data = old_packet->peekData<QuicData>(); // get data from packet
    StreamsData* streams_data = data->getStream_frames();
    std::vector<stream_frame*> frames = streams_data->getFramesArray();
    for (std::vector<stream_frame*>::iterator it =
            frames.begin(); it != frames.end(); ++it) {
            stream_frame* curr_frame = *it;
            int stream_id = curr_frame->stream_id;
            stream_arr->blockStream(stream_id);
    }

    Packet* new_packet = createQuicDataPacket(streams_data);
    packet_counter++;
    return new_packet;
}

void QuicConnectionClient::processRexmitTimer(){
    congestion_alg->processRexmitTimer();
}

void QuicConnectionClient::createRetransmitPacket(Packet* packet_to_remove) {
    send_not_ACKED_queue->remove(packet_to_remove);
    Packet* new_packet = updatePacketNumber(packet_to_remove);
    waiting_retransmission->push_back(new_packet);
}

} /* namespace inet */


