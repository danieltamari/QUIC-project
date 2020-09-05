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
    this->num_packets_sent = 0;
    this->send_una = 0;
    this->last_rcvd_ACK = 0;
    this->dup_ACKS = 0;
    this->destination=destination;

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
    return packet_to_send;
}


StreamsData* QuicConnectionClient::CreateSendData(int max_payload, int connection_window) {
    StreamsData *data_to_send = this->stream_arr->DataToSend(max_payload, connection_window);
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
    //QuicHeader->setClient_number(packet->getKind());//###### NEED TO CHANGE

    packet_to_send->insertAtFront(QuicHeader);
    this->SetSourceID(src_ID);
    this->SetDestID(dest_ID);

    const auto &payload = makeShared<QuicHandShakeData>();
    payload->setInitial_source_connection_id(src_ID);
    payload->setChunkLength(B(sizeof(int)*1));
    packet_to_send->insertAtBack(payload);

    this->event->setKind(QUIC_S_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE);
    return packet_to_send;
}




Packet* QuicConnectionClient::ProcessClientHandshakeResponse(Packet* packet) {

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

    StreamsData *send_data = this->CreateSendData(max_payload, connection_flow_control_recieve_window);
    Packet* packet_to_send = createQuicDataPacket(send_data);
    Packet* copy_packet_to_send=createQuicDataPacket(send_data);
    copy_packet_to_send->setTimestamp(simTime());

    packet_counter++;
    this->send_not_ACKED_queue.push_back(copy_packet_to_send);

    num_packets_sent++;
    this->event->setKind(QUIC_S_SEND);
    return packet_to_send;
}




Packet* QuicConnectionClient::ProcessClientSend(Packet* packet){
    auto header = packet->popAtFront<QuicPacketHeader>();
    int packet_num_ACKED = header->getPacket_number();
    Packet* copy_of_ACKED_packet = RemovePacketFromQueue(packet_num_ACKED);
    auto header_ = copy_of_ACKED_packet->popAtFront<QuicPacketHeader>();
    auto data = copy_of_ACKED_packet->peekData<QuicData>(); // get data from packet
    const StreamsData* streams_data = data->getStream_frames();
    uint32_t size_in_bytes = streams_data->getTotalSize();
    uint32_t old_send_una = send_una;
    send_una += size_in_bytes;
    congestion_alg->SetSndUnA(send_una);
    int sum_stream_window_size = this->stream_arr->getSumStreamsWindowSize();
    congestion_alg->SetSndWnd(std::min(connection_max_flow_control_window_size,sum_stream_window_size));

    // ack_number -> packet we expect to see next
    int ack_number = packet_num_ACKED + 1; // change later ACK frames
    if (last_rcvd_ACK == ack_number) {
        dup_ACKS++;
        congestion_alg->SetDupACKS(dup_ACKS);
        congestion_alg->receivedDuplicateAck();
    }
    else {
        last_rcvd_ACK = ack_number;
        dup_ACKS = 0;
        congestion_alg->SetDupACKS(dup_ACKS);
        congestion_alg->receivedDataAck(old_send_una);
    }


    StreamsData *send_data = this->CreateSendData(max_payload, connection_flow_control_recieve_window);
    Packet* packet_to_send = createQuicDataPacket(send_data);
    Packet* copy_packet_to_send=createQuicDataPacket(send_data);
    this->send_not_ACKED_queue.push_back(copy_packet_to_send);
    packet_counter++;
    num_packets_sent++;
    return packet_to_send;
}



Packet* QuicConnectionClient::RemovePacketFromQueue(int packet_number){
    Packet* packet_to_remove;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue.begin(); it != send_not_ACKED_queue.end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number()==packet_number){
                packet_to_remove=*it;
                send_not_ACKED_queue.remove(packet_to_remove);
                break;
            }
    }
    return packet_to_remove;
}

Packet* QuicConnectionClient::findPacket(int packet_number) {
    Packet* packet_to_peek;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue.begin(); it != send_not_ACKED_queue.end(); ++it) {
            auto current_header=(*it)->popAtFront<QuicPacketHeader>();
//            auto data=(*it)->peekData<QuicData>();
//            send_not_ACKED_queue.remove(*it);
//            Packet* pac = new Packet();
//            pac->insertAtFront(current_header);
//            pac->insertAtBack(data);
//            send_not_ACKED_queue.push_back(pac);
            if (current_header->getPacket_number()==packet_number){
                packet_to_peek=pac;
                break;
            }
    }
    return packet_to_peek;
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

    //update connection
    int sum_highest_offsets = this->GetMaxOffset();
    connection_flow_control_recieve_window = connection_flow_control_recieve_offset -sum_highest_offsets;

}

void QuicConnectionClient::UpdateRtt(simtime_t acked_time) {
    uint32 acked_time_int = this->congestion_alg->convertSimtimeToTS(acked_time);
    this->congestion_alg->rttMeasurementCompleteUsingTS(acked_time_int);
}

void QuicConnectionClient::processRexmitTimer(){
    congestion_alg->processRexmitTimer();
}

} /* namespace inet */


