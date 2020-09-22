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
#include "stdlib.h"

namespace inet {

QuicConnectionClient::QuicConnectionClient() {
    // TODO Auto-generated constructor stub

}

QuicConnectionClient::QuicConnectionClient(int* connection_data, int connection_data_size,L3Address destination) {

    stream_arr = new QuicStreamArr(connection_data_size);
    this->congestion_alg=new QuicNewReno();

    for (int i=0; i<connection_data_size; i++) {
        AddNewStream(connection_data[i],i);
    }
    packet_counter = 0;
    send_una = 0;
    last_rcvd_ACK = 0;
    dup_ACKS = 0;
    Bytes_in_flight = 0;
    this->destination=destination;
    send_not_ACKED_queue = new std::list<Packet*>();
    waiting_retransmission = new std::list<Packet*>();
    packets_to_send = new std::list<Packet*>();
    ACKED_out_of_order = new std::list<Packet*>();
}


QuicConnectionClient::~QuicConnectionClient() {
    // TODO Auto-generated destructor stub
}


void QuicConnectionClient::AddNewStream(int stream_size, int stream_id) {
    stream_arr->AddNewStream(stream_size, stream_id);
}


Packet* QuicConnectionClient::ProcessInitiateHandshake(Packet* packet) {
    char msgName[32];
    sprintf(msgName, "QUIC INITIAL HANDSHAKE");
    // get random values for connection IDs
    //int src_ID_ = std::rand();
    int dest_ID_ = std::rand();
    int src_ID_ = 0;
    // get length of the random numbers
    unsigned int dest_ID_len = calcSizeInBytes(dest_ID_);
    // unsigned int src_ID_len = calcSizeInBytes(src_ID_);
    unsigned int src_ID_len = 1;
    // create HANDSHAKE packet
    Packet *packet_to_send = new Packet(msgName);
    // create long header
    auto QuicHeader_ = makeShared<QuicLongHeader>();
    QuicHeader_->setHeader_form(b(1));
    QuicHeader_->setFixed_bit(b(1));
    QuicHeader_->setLong_packet_type(2);
    QuicHeader_->setVersion(1);
    QuicHeader_->setDest_connection_ID(dest_ID_);
    QuicHeader_->setDest_connection_id_length(dest_ID_len);
    QuicHeader_->setSource_connection_ID(src_ID_);
    QuicHeader_->setSource_connection_id_length(src_ID_len);
    int header_length = LONG_HEADER_BASE_LENGTH + dest_ID_len + src_ID_len;
    QuicHeader_->setChunkLength(B(header_length));
    packet_to_send->insertAtFront(QuicHeader_);
    this->SetSourceID(src_ID_);
    this->SetDestID(dest_ID_);

    // create frames array for packet's payload
    const auto &payload = makeShared<QuicFramesArray>();
    const auto &handshake_data = makeShared<QuicHandShakeData>();
    handshake_data->setInitial_source_connection_id(src_ID_);
    handshake_data->setChunkLength(B(sizeof(long)));
    const auto &padding = makeShared<PaddingFrame>();
    padding->setChunkLength(B(QUIC_ALLOWED_PACKET_SIZE) - handshake_data->getChunkLength() - QuicHeader_->getChunkLength());
    payload->setFrames_arrayArraySize(2);
    payload->setFrames_array(0, *handshake_data);
    payload->setFrames_array(1, *padding);
    payload->setChunkLength(B(QUIC_ALLOWED_PACKET_SIZE));
    packet_to_send->insertAtBack(payload);
    return packet_to_send;
}


void QuicConnectionClient::ProcessClientHandshakeResponse(Packet* packet) {
    auto header = packet->popAtFront<QuicLongHeader>();
    auto handshake_data = packet->peekData<QuicHandShakeData>();
    // auto frames_array = packet->peekData<QuicFramesArray>(); // get data from packet
    // auto handshake_data = dynamic_cast<QuicHandShakeData*>(frames_array->getFrames_array_pointer(0));
    // process data
    int initial_stream_window = handshake_data->getInitial_max_stream_data();
    int initial_connection_window = handshake_data->getInitial_max_data();
    int max_payload_ = handshake_data->getMax_udp_payload_size();
    max_payload = max_payload_;
    connection_max_flow_control_window_size = initial_connection_window;
    connection_flow_control_recieve_window = initial_connection_window;
    stream_arr->setAllStreamsWindows(initial_stream_window);
    congestion_alg->SetSndMss(max_payload);
    congestion_alg->SetSndCwnd(max_payload*2); //initial congestion window
    congestion_alg->SetSsThresh(max_payload*SSTHRESH_CHANGE_THIS); // initial slow start threshold.
    int sum_stream_window_size = this->stream_arr->getSumStreamsWindowSize();
    congestion_alg->SetSndWnd(std::min(connection_flow_control_recieve_window,sum_stream_window_size));
}


StreamsData* QuicConnectionClient::CreateSendData(int max_payload) {
    StreamsData *data_to_send = this->stream_arr->DataToSend(max_payload-QUIC_SHORT_HEADER_LENGTH);
    if (data_to_send == NULL)
        return NULL;
    int bytes_sent = data_to_send->getTotalSize();
//    congestion_alg->SetSndMax(bytes_sent);
    return data_to_send;
}



Packet* QuicConnectionClient::createQuicDataPacket(StreamsData* streams_data) {
    char msgName[32];
    sprintf(msgName, "QUIC packet number-%d", packet_counter);

    Packet *packet_to_send = new Packet(msgName);
    auto QuicHeader = makeShared<QuicPacketHeader>();
    QuicHeader->setPacket_number(packet_counter);
    QuicHeader->setSrc_connectionID(connection_source_ID);
    QuicHeader->setDest_connectionID(connection_dest_ID);
    QuicHeader->setPacket_type(4);
    QuicHeader->setChunkLength(B(sizeof(int)*4));
    QuicHeader->setHeader_form(b(0));
    packet_to_send->insertAtFront(QuicHeader);

    const auto &payload = makeShared<QuicData>();
  //  int msgByteLength = streams_data->getTotalSize();
    int msgByteLength = 1184;
    payload->setChunkLength(B(msgByteLength));
    payload->setStream_frames(streams_data);
    packet_to_send->insertAtBack(payload);
    packet_counter++;
    congestion_alg->SetSndMax(1200);
    return packet_to_send;
}



void QuicConnectionClient::ProcessClientSend(){

    updateBytesInFlight();
    EV << "############ congestion window size: " << congestion_alg->GetSndCwnd()  << " ############" << endl;
    EV << "############ flow control window size: " << connection_flow_control_recieve_window  << " ############"<< endl;


    int actual_window = std::min(connection_flow_control_recieve_window,congestion_alg->GetSndCwnd() - Bytes_in_flight);

    EV << " ############ actual window size: " << actual_window << " ############" << endl;
    EV << " ############ Bytes in flight: " << Bytes_in_flight << " ############"<< endl;

    if (actual_window<0)
       actual_window=0;
    int curr_payload_size = max_payload;

    // trying to send retransmissions first
    std::list<Packet*>::iterator it = waiting_retransmission->begin();
    while (it != waiting_retransmission->end()) {

        // print waiting_retransmission:
        EV << "########### waiting_retransmission: ###############" << endl;
        for (std::list<Packet*>::iterator it =
                waiting_retransmission->begin(); it != waiting_retransmission->end(); ++it) {
            auto temp_header =(*it)->peekAtFront<QuicPacketHeader>();
            EV << "packet number: " << temp_header->getPacket_number() << endl;
        }

        Packet* packet_to_send = *it;
        it++;
        waiting_retransmission->remove(packet_to_send);
        packets_to_send->push_back(packet_to_send);
      //  int packet_size = packet_to_send->getByteLength();
      //  actual_window -= packet_size;
     //   connection_flow_control_recieve_window -= packet_size;
        createCopyPacket(packet_to_send);
       // packet_counter++;
    }

    // create new data packets
    while (actual_window != 0) {
        if (actual_window < max_payload) {
            break;
        }

        StreamsData *send_data = this->CreateSendData(curr_payload_size);
        if (send_data == NULL)
            break; // all streams are blocked


        Packet* packet_to_send = createQuicDataPacket(send_data);
        retransmission_info* info = new retransmission_info;
        info->setIs_retransmit(false);
        info->setName("info");
        packet_to_send->addObject(info);
        packets_to_send->push_back(packet_to_send);
        // update flow control window
        int packet_size = packet_to_send->getByteLength();
        actual_window -= packet_size;
        connection_flow_control_recieve_window -= packet_size;
        createCopyPacket(packet_to_send);
    }

    updateBytesInFlight();
}



void QuicConnectionClient::ProcessClientACK(Packet* ack_packet, packet_rcv_type* acked_packet_arr,int total_acked){
    auto header = ack_packet->popAtFront<QuicPacketHeader>();
    auto ACK_data = ack_packet->peekData<QuicACKFrame>();

    // if first is in order, remove the packets previous to it from send_not_ACKED and update flow & congestion
    int first_acked = acked_packet_arr[total_acked-1].packet_number;
    if (acked_packet_arr[total_acked-1].in_order) {
        std::list<Packet*>::iterator it = send_not_ACKED_queue->begin();
        while (it != send_not_ACKED_queue->end()) {
                auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
                if (current_header->getPacket_number() < first_acked){
                    Packet* copy_of_ACKED_packet = (*it);
                    it++;
                    send_not_ACKED_queue->remove(copy_of_ACKED_packet);
                    Packet* p_temp = copy_of_ACKED_packet->dup();
                    updateCongestionControl(copy_of_ACKED_packet);
                    updateFlowControl(copy_of_ACKED_packet);
                    freeBlockedStreams(p_temp);
                }
                else
                    it++;
        }
    }

    // update flow control and remove all previous OOO packet until the first one ACKED
    std::list<Packet*>::iterator it = ACKED_out_of_order->begin();
    while (it != ACKED_out_of_order->end()) {
        auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
        Packet* packet_to_remove;
        if (current_header->getPacket_number() < first_acked){
            packet_to_remove=*it;
            it++;
            ACKED_out_of_order->remove(packet_to_remove);
            Packet* p_temp = packet_to_remove->dup();
            updateFlowControl(packet_to_remove);
            freeBlockedStreams(p_temp);
        }
        else
            it++;
    }

    // go over all the ACKED packets in this ACK frame
    for (int i = 0; i < total_acked; i++) {
        int current_packet = acked_packet_arr[i].packet_number;
        Packet* copy_of_ACKED_packet = RemovePacketFromQueue(current_packet);
        if (copy_of_ACKED_packet != NULL) {
            Packet* p_temp = copy_of_ACKED_packet->dup();
            updateCongestionControl(copy_of_ACKED_packet);
            if (acked_packet_arr[i].in_order) {
                updateFlowControl(p_temp);
            }

            else
                ACKED_out_of_order->push_back(p_temp);

            freeBlockedStreams(copy_of_ACKED_packet);
        }
    }

    // print send_not_ACKED_queue:
    EV << "########### send_not_ACKED_queue: ###############" << endl;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
        auto temp_header =(*it)->peekAtFront<QuicPacketHeader>();
        EV << "packet number: " << temp_header->getPacket_number() << endl;
    }

    // print ACKED_out_of_order:
    EV << "############### ACKED_out_of_order: ################" << endl;
    for (std::list<Packet*>::iterator it =
            ACKED_out_of_order->begin(); it != ACKED_out_of_order->end(); ++it) {
        auto temp_header =(*it)->peekAtFront<QuicPacketHeader>();
        EV << "packet number: " << temp_header->getPacket_number() << endl;
    }

    updateBytesInFlight();
}

void QuicConnectionClient::createCopyPacket(Packet* original_packet) {
    // create copy
    Packet* copy_packet_to_send = original_packet->dup();
    copy_packet_to_send->setTimestamp(simTime());
    this->send_not_ACKED_queue->push_back(copy_packet_to_send);
}





Packet* QuicConnectionClient::RemovePacketFromQueue(int packet_number){
    Packet* packet_to_remove = NULL;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number()==packet_number){
                packet_to_remove=*it;
                //send_not_ACKED_queue->remove(packet_to_remove);
                send_not_ACKED_queue->erase(it);
              //  send_not_ACKED_queue->
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

void QuicConnectionClient::updateFlowControl (Packet* acked_packet){
    auto header_ = acked_packet->popAtFront<QuicPacketHeader>();
    auto data = acked_packet->peekData<QuicData>(); // get data from packet
    const StreamsData* streams_data = data->getStream_frames();
    std::vector<stream_frame*> acked_frames = streams_data->getFramesArray();
    uint32_t size_in_bytes = streams_data->getTotalSize() + QUIC_SHORT_HEADER_LENGTH;

    // if no OOO frames -> update streams & connection flow control windows
    connection_flow_control_recieve_window += size_in_bytes;
    for (std::vector<stream_frame*>::iterator it =
            acked_frames.begin(); it != acked_frames.end(); ++it) {
        int stream_id = (*it)->stream_id;
        int length = (*it)->length;
        stream_arr->updateStreamFlowWindow(stream_id,length);
    }
}

void QuicConnectionClient::updateCongestionControl (Packet* copy_of_ACKED_packet) {
    simtime_t acked_time = copy_of_ACKED_packet->getTimestamp();
    UpdateRtt(acked_time); //update rtt measurement
    // update congestion control
    int size_in_bytes = copy_of_ACKED_packet->getByteLength();
    old_send_una = send_una;
    send_una += size_in_bytes;
    congestion_alg->SetSndUnA(send_una);
    int sum_stream_window_size = stream_arr->getSumStreamsWindowSize();
    congestion_alg->SetSndWnd(std::min(connection_flow_control_recieve_window,sum_stream_window_size));
}


void QuicConnectionClient::UpdateRtt(simtime_t acked_time) {
    uint32 acked_time_int = this->congestion_alg->convertSimtimeToTS(acked_time);
    this->congestion_alg->rttMeasurementCompleteUsingTS(acked_time_int);
}

Packet* QuicConnectionClient::updatePacketNumber(Packet* old_packet) {
    send_not_ACKED_queue->remove(old_packet);
    auto current_header=old_packet->popAtFront<QuicPacketHeader>();
    int old_packet_number = current_header->getPacket_number();
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
   // packet_counter++;

    retransmission_info* info = new retransmission_info;
    info->setIs_retransmit(true);
    info->setOriginal_packet_number(old_packet_number);
    info->setName("info");
    new_packet->addObject(info);

    auto temp_header = new_packet->peekAtFront<QuicPacketHeader>();
    EV << "################retransmission info: ############" << endl;
    EV << "packet " << temp_header->getPacket_number() << " is retransmission of " << old_packet_number << endl;
    return new_packet;
}


void QuicConnectionClient::updateBytesInFlight() {
    this->Bytes_in_flight = 0;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
            this->Bytes_in_flight += (*it)->getByteLength();
    }
}

void QuicConnectionClient::freeBlockedStreams(Packet* copy_of_ACKED_packet) {
    cObject* p = copy_of_ACKED_packet->getObject("info");
    retransmission_info* info = dynamic_cast<retransmission_info*>(p);
    if (info->getIs_retransmit()) {
        auto curr_header = copy_of_ACKED_packet->popAtFront<QuicPacketHeader>();
        auto data = copy_of_ACKED_packet->peekData<QuicData>();
        const StreamsData* streams_data = data->getStream_frames();
        std::vector<stream_frame*> frames = streams_data->getFramesArray();
        for (std::vector<stream_frame*>::iterator it =
                frames.begin(); it != frames.end(); ++it) {
                int stream_id = (*it)->stream_id;
                stream_arr->freeStream(stream_id);
        }

    }

}

void QuicConnectionClient::processRexmitTimer(){
    congestion_alg->processRexmitTimer();
}


void QuicConnectionClient::createRetransmitPacket(Packet* packet_to_remove) {

    Packet* new_packet = updatePacketNumber(packet_to_remove);
    waiting_retransmission->push_back(new_packet);
}


} /* namespace inet */


