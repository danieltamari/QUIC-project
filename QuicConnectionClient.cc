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
#include "QuicConnectionClient.h"

namespace inet {

QuicConnectionClient::QuicConnectionClient() {
    // TODO Auto-generated constructor stub

}

QuicConnectionClient::QuicConnectionClient(int* connection_data, int connection_data_size,L3Address destination,bool reconnect) {
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
    this->reconnect=reconnect;
}


QuicConnectionClient::~QuicConnectionClient() {
    // TODO Auto-generated destructor stub
}


void QuicConnectionClient::AddNewStream(int stream_size, int stream_id) {
    stream_arr->AddNewStream(stream_size, stream_id);
}


Packet* QuicConnectionClient::ProcessInitiateHandshake(int src_ID_) {
    char msgName[32];
    sprintf(msgName, "QUIC INITIAL PACKET");
    // get random values for dest connection ID
    int dest_ID_ = std::rand();
    // get length of the random numbers
    unsigned int dest_ID_len = calcSizeInBytes(dest_ID_);
    unsigned int src_ID_len = calcSizeInBytes(src_ID_);
    // create HANDSHAKE packet
    Packet *packet_to_send = new Packet(msgName);
    // create long header
    auto QuicHeader_ = makeShared<QuicLongHeader>();
    QuicHeader_->setHeader_form(b(1));
    QuicHeader_->setFixed_bit(b(1));
    QuicHeader_->setLong_packet_type(0);
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

    // create frames for packet's payload
    const auto &handshake_data = makeShared<QuicHandShakeData>();
    handshake_data->setInitial_source_connection_id(src_ID_);
    handshake_data->setChunkLength(B(sizeof(long)));
    const auto &padding = makeShared<PaddingFrame>();
    padding->setChunkLength(B(QUIC_ALLOWED_PACKET_SIZE) - handshake_data->getChunkLength() - QuicHeader_->getChunkLength());
    packet_to_send->insertAtBack(handshake_data);
    packet_to_send->insertAtBack(padding);
    return packet_to_send;
}


void QuicConnectionClient::ProcessClientHandshakeResponse(Packet* packet) {
    auto long_header = packet->popAtFront<QuicPacketHeader>();
    auto handshake_data = packet->peekDataAt<QuicHandShakeData>(b(0));
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
    congestion_alg->SetSndWnd(std::min(connection_flow_control_recieve_window-Bytes_in_flight,sum_stream_window_size));
    delete packet;
}


Packet* QuicConnectionClient::createQuicDataPacket(std::vector<IntrusivePtr<StreamFrame>>* frames_to_send, int total_size) {
        char msgName[32];
        sprintf(msgName, "QUIC packet number-%d", packet_counter);

        Packet *packet_to_send = new Packet(msgName);
        // create short header
       // unsigned int packet_number_len = calcSizeInBytes(packet_counter);
        unsigned int packet_number_len = 2;
        unsigned int dest_ID_len = calcSizeInBytes(connection_dest_ID);
        auto QuicHeader_ = makeShared<QuicShortHeader>();
        QuicHeader_->setHeader_form(b(0));
        QuicHeader_->setDest_connection_ID(connection_dest_ID);
        QuicHeader_->setPacket_number_length(B(packet_number_len));
        QuicHeader_->setPacket_number(packet_counter);
      //  QuicHeader_->setPacket_type(4);
        int header_length = SHORT_HEADER_BASE_LENGTH + dest_ID_len + packet_number_len;
        QuicHeader_->setChunkLength(B(header_length));
        packet_to_send->insertAtFront(QuicHeader_);

        for (std::vector<IntrusivePtr<StreamFrame>>::iterator it =
                frames_to_send->begin(); it != frames_to_send->end(); ++it) {
            packet_to_send->insertAtBack(*it);
        }

        if (total_size < QUIC_ALLOWED_PACKET_SIZE) { // add padding frame
            const auto &padding = makeShared<PaddingFrame>();
            padding->setChunkLength(B(QUIC_ALLOWED_PACKET_SIZE - total_size));
            packet_to_send->insertAtBack(padding);
        }

        packet_counter++;
        return packet_to_send;
}


void QuicConnectionClient::ProcessClientSend(){

    EV << "############ congestion window size: " << congestion_alg->GetSndCwnd()  << " ############" << endl;
    EV << "############ flow control window size: " << connection_flow_control_recieve_window - Bytes_in_flight << " ############"<< endl;


    int actual_window = std::min(connection_flow_control_recieve_window - Bytes_in_flight,congestion_alg->GetSndCwnd() - Bytes_in_flight);

    EV << " ############ actual window size: " << actual_window << " ############" << endl;
    EV << " ############ Bytes in flight: " << Bytes_in_flight << " ############"<< endl;

    if (actual_window < 0)
       actual_window = 0;
    int curr_payload_size = max_payload;

    // trying to send retransmissions first
    std::list<Packet*>::iterator it = waiting_retransmission->begin();
    while (it != waiting_retransmission->end()) {

        // print waiting_retransmission:
        EV << "########### waiting_retransmission: ###############" << endl;
        for (std::list<Packet*>::iterator it =
                waiting_retransmission->begin(); it != waiting_retransmission->end(); ++it) {
            auto temp_header =(*it)->peekAtFront<QuicShortHeader>();
            EV << "packet number: " << temp_header->getPacket_number() << endl;
        }
        Packet* packet_to_send = *it;
        it++;
        waiting_retransmission->remove(packet_to_send);
        packets_to_send->push_back(packet_to_send);
        createCopyPacket(packet_to_send);
    }

    int short_header_size = calcHeaderSize();
    // create new data packets
    while (actual_window != 0) {
        // if window is less than 1 packet size - don't send anything
        if (actual_window < max_payload) {
            break;
        }

      //  StreamsData *send_data = this->CreateSendData(curr_payload_size, short_header_size);
       // if (send_data == NULL)
      //      break; // all streams are blocked

        std::vector<IntrusivePtr<StreamFrame>>* frames_to_send = stream_arr->framesToSend(curr_payload_size - short_header_size);
        if (frames_to_send->empty())
            break; // currently there is no data to send
        int frames_total_size = calcTotalSize(frames_to_send);
        int bytes_sent = frames_total_size + short_header_size;
        // congestion_alg->SetSndMax(bytes_sent);

        Packet* packet_to_send = createQuicDataPacket(frames_to_send, bytes_sent);
        retransmission_info* info = new retransmission_info;
        info->setIs_retransmit(false);
        info->setName("info");
        packet_to_send->addObject(info);
        packets_to_send->push_back(packet_to_send);
        // update flow control window
        int packet_size = packet_to_send->getByteLength();
        actual_window -= packet_size;
        congestion_alg->SetSndMax(packet_size);
       // connection_flow_control_recieve_window -= packet_size;
        createCopyPacket(packet_to_send);
        Bytes_in_flight += packet_size;
        congestion_alg->SetFlightSize(Bytes_in_flight);
    }
}



void QuicConnectionClient::ProcessClientACK(packet_rcv_type* acked_packet_arr,int total_acked){
   // auto header = ack_packet->popAtFront<QuicPacketHeader>();
   // auto ACK_data = ack_packet->peekData<ACKFrame>();

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
                    int packet_size = copy_of_ACKED_packet->getByteLength();
                    Bytes_in_flight -= packet_size;
                    congestion_alg->SetFlightSize(Bytes_in_flight);
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
            int packet_size = copy_of_ACKED_packet->getByteLength();
            Bytes_in_flight -= packet_size;
            congestion_alg->SetFlightSize(Bytes_in_flight);
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

   // updateBytesInFlight();
}


int QuicConnectionClient::calcTotalSize(std::vector<IntrusivePtr<StreamFrame>>* frames_to_send) {
    int total_bytes = 0;
    for (std::vector<IntrusivePtr<StreamFrame>>::iterator it =
            frames_to_send->begin(); it != frames_to_send->end(); ++it) {
        total_bytes += (*it)->getLength();
    }
    return total_bytes;
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
                send_not_ACKED_queue->erase(it);
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

bool QuicConnectionClient::GetReconnect(){
    return reconnect;
}
void QuicConnectionClient::updateFlowControl (Packet* acked_packet){
    auto header_ = acked_packet->popAtFront<QuicPacketHeader>();
    // process frames in acked packets
    b offset = b(0);
    while (offset < acked_packet->getDataLength()) {
        auto curr_frame = acked_packet->peekDataAt<QuicFrame>(offset);
        int type = curr_frame->getFrame_type();
        if (type == 5) { // temporary - until receiving different types of frames
            auto stream_frame = acked_packet->peekDataAt<StreamFrame>(offset);
           //offset += stream_frame->getLength();
            // if no OOO frames -> update streams connection flow control windows
            int stream_id = stream_frame->getStream_id();
            int length = stream_frame->getLength();
            stream_arr->updateStreamFlowWindow(stream_id,length);
        }
        offset += curr_frame->getChunkLength();
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
    congestion_alg->SetSndWnd(std::min(connection_flow_control_recieve_window-Bytes_in_flight,sum_stream_window_size));
}


void QuicConnectionClient::UpdateRtt(simtime_t acked_time) {
    uint32 acked_time_int = congestion_alg->convertSimtimeToTS(acked_time);
    congestion_alg->rttMeasurementCompleteUsingTS(acked_time_int);
}

Packet* QuicConnectionClient::updatePacketNumber(Packet* old_packet) {
    send_not_ACKED_queue->remove(old_packet);
    auto current_header=old_packet->popAtFront<QuicPacketHeader>();
    int old_packet_number = current_header->getPacket_number();
    std::vector<IntrusivePtr<StreamFrame>>* frames_to_retransmit = new std::vector<IntrusivePtr<StreamFrame>>();
    b offset = b(0);
    int total_size = 0;
    while (offset < old_packet->getDataLength()) {
        auto curr_frame = old_packet->peekDataAt<QuicFrame>(offset);
        int type = curr_frame->getFrame_type();
        if (type == 5) { // temporary - until receiving different types of frames
            auto stream_frame = old_packet->peekDataAt<StreamFrame>(offset);
            //offset += stream_frame->getLength();
            // block relevant streams
            int stream_id = stream_frame->getStream_id();
            stream_arr->blockStream(stream_id);
            // add frame to new frames vector
            const auto &new_stream_frame = makeShared<StreamFrame>();
            new_stream_frame->setStream_id(stream_id);
            new_stream_frame->setOffset(stream_frame->getOffset());
            new_stream_frame->setLength(stream_frame->getLength());
            new_stream_frame->setIs_FIN(stream_frame->getIs_FIN());
            new_stream_frame->setFrame_type(5);
            new_stream_frame->setChunkLength(B(stream_frame->getLength()));
            frames_to_retransmit->push_back(new_stream_frame);
            total_size += stream_frame->getLength();
        }
        offset += curr_frame->getChunkLength();
    }

    // create new packet to send
    int short_header_size = calcHeaderSize();
    Packet* new_packet = createQuicDataPacket(frames_to_retransmit, total_size + short_header_size);

    // find out if packet is retransmission
    cObject* p = old_packet->getObject("info");
    retransmission_info* old_info = dynamic_cast<retransmission_info*>(p);
    retransmission_info* info = new retransmission_info;
    info->setIs_retransmit(true);
    std::vector<int> new_numbers;
    if (old_info->getIs_retransmit()) {
        std::vector<int> numbers_to_add = old_info->getPackets_numbers();
        new_numbers.insert(std::end(new_numbers), std::begin(numbers_to_add), std::end(numbers_to_add));
    }
    new_numbers.push_back(old_packet_number);
    info->setPackets_numbers(new_numbers);
    info->setName("info");
    new_packet->addObject(info);

    auto temp_header = new_packet->peekAtFront<QuicPacketHeader>();
    EV << "################retransmission info: ############" << endl;
    EV << "packet " << temp_header->getPacket_number() << " is retransmission of " << old_packet_number << endl;
    return new_packet;
}


void QuicConnectionClient::freeBlockedStreams(Packet* copy_of_ACKED_packet) {
    cObject* p = copy_of_ACKED_packet->getObject("info");
    retransmission_info* info = dynamic_cast<retransmission_info*>(p);
    if (info->getIs_retransmit()) {
        auto curr_header = copy_of_ACKED_packet->popAtFront<QuicPacketHeader>();
        b offset = b(0);
        while (offset < copy_of_ACKED_packet->getDataLength()) {
            auto curr_frame = copy_of_ACKED_packet->peekDataAt<QuicFrame>(offset);
            int type = curr_frame->getFrame_type();
            if (type == 5) { // temporary - until receiving different types of frames
                auto stream_frame = copy_of_ACKED_packet->peekDataAt<StreamFrame>(offset);
                //offset += stream_frame->getLength();
                // free relevant streams
                int stream_id = stream_frame->getStream_id();
                stream_arr->freeStream(stream_id);
            }
            offset += curr_frame->getChunkLength();
        }
    }
}

int QuicConnectionClient::calcHeaderSize() {
  //  unsigned int packet_number_len = calcSizeInBytes(packet_counter);
    unsigned int packet_number_len = 2;
    unsigned int dest_ID_len = calcSizeInBytes(connection_dest_ID);
    return SHORT_HEADER_BASE_LENGTH + dest_ID_len + packet_number_len;
}


void QuicConnectionClient::processRexmitTimer(){
    congestion_alg->processRexmitTimer();
}


void QuicConnectionClient::createRetransmitPacket(Packet* packet_to_remove) {
    Packet* new_packet = updatePacketNumber(packet_to_remove);
    waiting_retransmission->push_back(new_packet);
}


} /* namespace inet */


