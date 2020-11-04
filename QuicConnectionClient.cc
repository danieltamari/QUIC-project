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
#include <algorithm>
#include "QuicConnectionClient.h"

namespace inet {


QuicConnectionClient::QuicConnectionClient() {
    // TODO Auto-generated constructor stub
}


QuicConnectionClient::QuicConnectionClient(int* connection_data, int connection_data_size,L3Address destination_,bool reconnect_) {
    stream_arr = new QuicStreamArr(connection_data_size);
    congestion_alg = new QuicNewReno();
    send_queue = new QuicSendQueue();
    waiting_retransmission = new std::list<Packet*>();
    packets_to_send = new std::list<Packet*>();
    for (int i = 0; i < connection_data_size; i++) {
        AddNewStream(connection_data[i],i);
    }
    packet_counter = 0;
    send_una = 0;
    last_rcvd_ACK = 0;
    dup_ACKS = 0;
    Bytes_in_flight = 0;
    destination = destination_;
    ACKED_out_of_order = new std::list<Packet*>();
    reconnect = reconnect_;
    num_bytes_sent = 0;
    bytes_sent_with_ret = 0;
    min_packet_lost = -1;
    recovery_start_packet = 0;
    current_sent_bytes = 0;
    current_sent_bytes_with_ret = 0;
}


QuicConnectionClient::~QuicConnectionClient() {
    // TODO Auto-generated destructor stub
}


void QuicConnectionClient::AddNewStream(int stream_size, int stream_id) {
    stream_arr->addNewStream(stream_size, stream_id);
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
    QuicHeader_->setPacket_number(-1);
    int header_length = LONG_HEADER_BASE_LENGTH + dest_ID_len + src_ID_len;
    QuicHeader_->setChunkLength(B(header_length));
    packet_to_send->insertAtFront(QuicHeader_);
    setSourceID(src_ID_);
    setDestID(dest_ID_);

    // create frames for packet's payload
    const auto &handshake_data = makeShared<QuicHandShakeData>();
    handshake_data->setInitial_source_connection_id(src_ID_);
    handshake_data->setChunkLength(B(sizeof(long)));
    const auto &padding = makeShared<PaddingFrame>();
    padding->setChunkLength(B(QUIC_MIN_PACKET_SIZE) - handshake_data->getChunkLength() - QuicHeader_->getChunkLength());
    packet_to_send->insertAtBack(handshake_data);
    packet_to_send->insertAtBack(padding);

    // create copy
    createCopyPacket(packet_to_send);
    return packet_to_send;
}


void QuicConnectionClient::ProcessClientHandshakeResponse(Packet* packet) {
    auto long_header = packet->popAtFront<QuicPacketHeader>();
    auto handshake_data = packet->peekDataAt<QuicHandShakeData>(b(0));
    Packet* initial_packet_copy = findInitialPacket();
    send_queue->removeFromSendQueue(initial_packet_copy);
    // process data
    int initial_stream_window = handshake_data->getInitial_max_stream_data();
    int initial_connection_window = handshake_data->getInitial_max_data();
    int max_payload_ = handshake_data->getMax_udp_payload_size();
    max_payload = max_payload_;
    connection_max_flow_control_window_size = initial_connection_window;
    connection_flow_control_recieve_window = initial_connection_window;
    stream_arr->setAllStreamsWindows(initial_stream_window);
    stream_flow_control_window = initial_stream_window;
    congestion_alg->setSndMss(max_payload);
    congestion_alg->setSndCwnd(max_payload * 2); //initial congestion window
    congestion_alg->setSsThresh(MAX_THRESHOLD); // initial slow start threshold.
    int sum_stream_window_size = stream_arr->getSumStreamsWindowSize();
    congestion_alg->setSndWnd(std::min(connection_flow_control_recieve_window,sum_stream_window_size));
    delete packet;
}


void QuicConnectionClient::ProcessClientSend(){

    EV << "############ congestion window size: " << congestion_alg->getSndCwnd()  << " ############" << endl;
    EV << "############ flow control window size: " << connection_flow_control_recieve_window - Bytes_in_flight << " ############"<< endl;

    int temp = congestion_alg->getSndCwnd();
    int actual_window = std::min(connection_flow_control_recieve_window - Bytes_in_flight,congestion_alg->getSndCwnd() - Bytes_in_flight);

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
        int packet_size = packet_to_send->getByteLength();
        bytes_sent_with_ret += packet_size;
        current_sent_bytes_with_ret += packet_size;
    }

    int short_header_size = calcHeaderSize(true);
    // create new data packets
    while (actual_window != 0) {
        // if window is less than 1 packet size - don't send anything
        if (actual_window < max_payload) {
            break;
        }

        std::vector<IntrusivePtr<StreamFrame>>* frames_to_send = stream_arr->framesToSend(curr_payload_size - short_header_size);
        if (frames_to_send->empty())
            break; // currently there is no data to send
        int frames_total_size = calcTotalSize(frames_to_send);
        int bytes_sent = frames_total_size + short_header_size;
        // congestion_alg->setSndMax(bytes_sent);

        Packet* packet_to_send = createQuicDataPacket(frames_to_send, bytes_sent, true);
        retransmission_info* info = new retransmission_info;
        info->setIs_retransmit(false);
        info->setName("info");
        packet_to_send->addObject(info);
        packets_to_send->push_back(packet_to_send);
        // update flow control window
        int packet_size = packet_to_send->getByteLength();
        actual_window -= packet_size;
        congestion_alg->setSndMax(packet_size);
       // connection_flow_control_recieve_window -= packet_size;
        createCopyPacket(packet_to_send);
        Bytes_in_flight += packet_size;
      //  congestion_alg->SetFlightSize(Bytes_in_flight);
        num_bytes_sent += frames_total_size;
        current_sent_bytes += packet_size;
        current_sent_bytes_with_ret += packet_size;
        bytes_sent_with_ret += packet_size;
    }
}


void QuicConnectionClient::ProcessClientZeroRtt(int connection_window_size, int max_payload_, int stream_window_size) {

    congestion_alg->setSndCwnd(max_payload_*2);
    connection_flow_control_recieve_window = connection_window_size;
    stream_arr->setAllStreamsWindows(stream_window_size);
    max_payload = max_payload_;
    int actual_window = std::min(connection_window_size - Bytes_in_flight,congestion_alg->getSndCwnd() - Bytes_in_flight);
    int long_header_size = calcHeaderSize(false);
    int curr_payload_size = max_payload_;
    // create new data packets
    while (actual_window != 0) {
        std::vector<IntrusivePtr<StreamFrame>>* frames_to_send = stream_arr->framesToSend(curr_payload_size - long_header_size);
        if (frames_to_send->empty())
            break; // currently there is no data to send
        int frames_total_size = calcTotalSize(frames_to_send);
        int bytes_sent = frames_total_size + long_header_size;
        // congestion_alg->setSndMax(bytes_sent);

        Packet* packet_to_send = createQuicDataPacket(frames_to_send, bytes_sent, false);
        retransmission_info* info = new retransmission_info;
        info->setIs_retransmit(false);
        info->setName("info");
        packet_to_send->addObject(info);
        packets_to_send->push_back(packet_to_send);
        current_sent_bytes += packet_to_send->getByteLength();
        // update flow control window
        int packet_size = packet_to_send->getByteLength();
        actual_window -= packet_size;
        congestion_alg->setSndMax(packet_size);
       // connection_flow_control_recieve_window -= packet_size;
        createCopyPacket(packet_to_send);
        Bytes_in_flight += packet_size;
     //   congestion_alg->SetFlightSize(Bytes_in_flight);
        num_bytes_sent += frames_total_size;
        bytes_sent_with_ret += packet_size;
        current_sent_bytes_with_ret += packet_size;
    }
}



void QuicConnectionClient::ProcessClientACK(packet_rcv_type* acked_packet_arr,int total_acked){
    // if first is in order, remove the packets previous to it from send_not_ACKED and update flow & congestion
    int first_acked = acked_packet_arr[total_acked-1].packet_number;
    int largest_in_order = acked_packet_arr[0].packet_number;
    // go over all acknowledged packets
    for (int i = 0; i < total_acked; i++) {
        int current_packet = acked_packet_arr[i].packet_number;
        Packet* copy_of_ACKED_packet = send_queue->removeFromSendQueueByNumber(current_packet);
        if (copy_of_ACKED_packet != NULL) {
            int packet_size = copy_of_ACKED_packet->getByteLength();
            Bytes_in_flight -= packet_size;
            Packet* p_temp = copy_of_ACKED_packet->dup();
            updateStreamInfo(copy_of_ACKED_packet);
            if (acked_packet_arr[total_acked-1].in_order) {
                if (i == 0) {
                    // update RTT on largest packet in this ACK frame
                    simtime_t acked_time = copy_of_ACKED_packet->getTimestamp();
                    updateRtt(acked_time); //update rtt measurement
                }
                updateFlowControl(p_temp);
                if (ACKED_out_of_order->empty()) {
                    // all missing packets arrived - update send_una
                    int size_in_bytes = p_temp->getByteLength();
                    send_una += size_in_bytes;
                    congestion_alg->setSndUna(send_una);
                    EV << "############### send_una in order: " << send_una <<  " ################" << endl;
                }
            }
            else
                ACKED_out_of_order->push_back(p_temp);
        }
    }

    // update flow control and remove all previous OOO packet until the first one ACKED from ACKED_out_of_order queue
    int number_to_remove_up_to;
    if (acked_packet_arr[total_acked-1].in_order)
        // if the smallest packet arrived in order - remove all up to the largest acknowledged
        number_to_remove_up_to = largest_in_order;

    else
        number_to_remove_up_to = first_acked;

    std::list<Packet*>::iterator it = ACKED_out_of_order->begin();
    while (it != ACKED_out_of_order->end()) {
        auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
        Packet* packet_to_remove;
        if (current_header->getPacket_number() < number_to_remove_up_to){
            packet_to_remove=*it;
            it++;
            ACKED_out_of_order->remove(packet_to_remove);
            // update send una
            int size_in_bytes = packet_to_remove->getByteLength();
            send_una += size_in_bytes;
            congestion_alg->setSndUna(send_una);
            EV << "############### send_una in partial ack: " << send_una <<  " ################" << endl;
            Packet* p_temp = packet_to_remove->dup();
            updateFlowControl(packet_to_remove);

        }
        else
            it++;
    }

    send_queue->printSendNotAcked();

    // print ACKED_out_of_order:
    EV << "############### ACKED_out_of_order: ################" << endl;
    for (std::list<Packet*>::iterator it =
            ACKED_out_of_order->begin(); it != ACKED_out_of_order->end(); ++it) {
        auto temp_header =(*it)->peekAtFront<QuicPacketHeader>();
        EV << "packet number: " << temp_header->getPacket_number() << endl;
    }
}


Packet* QuicConnectionClient::createQuicDataPacket(std::vector<IntrusivePtr<StreamFrame>>* frames_to_send, int total_size, bool one_RTT) {
        char msgName[32];
        sprintf(msgName, "QUIC packet number-%d", packet_counter);

        Packet *packet_to_send = new Packet(msgName);
        if (one_RTT) {
            // create short header
            unsigned int packet_number_len = 2;
            unsigned int dest_ID_len = calcSizeInBytes(connection_dest_ID);
            auto QuicHeader_ = makeShared<QuicShortHeader>();
            QuicHeader_->setHeader_form(b(0));
            QuicHeader_->setDest_connection_ID(connection_dest_ID);
            QuicHeader_->setPacket_number_length(B(packet_number_len));
            QuicHeader_->setPacket_number(packet_counter);
            int header_length = SHORT_HEADER_BASE_LENGTH + dest_ID_len + packet_number_len;
            QuicHeader_->setChunkLength(B(header_length));
            packet_to_send->insertAtFront(QuicHeader_);
        }

        else { // 0-RTT
            // create long header
            unsigned int dest_ID_len = calcSizeInBytes(connection_dest_ID);
            unsigned int src_ID_len = calcSizeInBytes(connection_source_ID);
            unsigned int packet_number_len = 2;
            auto QuicHeader_ = makeShared<QuicLongHeader>();
            QuicHeader_->setHeader_form(b(1));
            QuicHeader_->setFixed_bit(b(1));
            QuicHeader_->setLong_packet_type(1);
            QuicHeader_->setVersion(1);
            QuicHeader_->setDest_connection_ID(connection_dest_ID);
            QuicHeader_->setDest_connection_id_length(dest_ID_len);
            QuicHeader_->setSource_connection_ID(connection_source_ID);
            QuicHeader_->setSource_connection_id_length(src_ID_len);
            QuicHeader_->setPacket_number(packet_counter);
            int header_length = LONG_HEADER_BASE_LENGTH + dest_ID_len + src_ID_len + packet_number_len;
            QuicHeader_->setChunkLength(B(header_length));
            packet_to_send->insertAtFront(QuicHeader_);

        }

        for (std::vector<IntrusivePtr<StreamFrame>>::iterator it =
                frames_to_send->begin(); it != frames_to_send->end(); ++it) {
            packet_to_send->insertAtBack(*it);
        }

        if (total_size < QUIC_MIN_PACKET_SIZE) { // add padding frame
            const auto &padding = makeShared<PaddingFrame>();
            padding->setChunkLength(B(QUIC_MIN_PACKET_SIZE - total_size));
            packet_to_send->insertAtBack(padding);
        }

        packet_counter++;
       // packet_to_send->setTimestamp(simTime());
        return packet_to_send;
}


void QuicConnectionClient::createCopyPacket(Packet* original_packet) {
    // create copy
    send_queue->createCopyPacket(original_packet);
}


Packet* QuicConnectionClient::createRetransmitPacket(Packet* old_packet, bool immedidate_retransmit) {
    send_queue->removeFromSendQueue(old_packet);
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
    Packet* new_packet;
    if (current_header->getHeader_form() == b(0)) { // short header -> 1-RTT
        int short_header_size = calcHeaderSize(true);
        new_packet = createQuicDataPacket(frames_to_retransmit, total_size + short_header_size, true);
    }

    else { // long header -> 0-RTT
        int long_header_size = calcHeaderSize(false);
        new_packet = createQuicDataPacket(frames_to_retransmit, total_size + long_header_size, false);
    }

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

    if (!immedidate_retransmit)
        waiting_retransmission->push_back(new_packet);
    return new_packet;
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


void QuicConnectionClient::updateStreamFlowControl (int  stream_id,int flow_control_window){
    stream_arr->updateStreamFlowWindow(stream_id, flow_control_window);
}


void QuicConnectionClient::updateRtt(simtime_t acked_time) {
    uint32 acked_time_int = congestion_alg->convertSimtimeToTS(acked_time);
    congestion_alg->rttMeasurementCompleteUsingTS(acked_time_int);
}


void QuicConnectionClient::updateCongestionAlgo(std::vector<int>* lost_packets_numbers, int largest) {
    bool recovery = congestion_alg->getLossRecovery();
    if (lost_packets_numbers->empty() && !recovery) {
        congestion_alg->receivedDataAck();
    }

    else if(!recovery) {
        int min_packet_lost = *(std::min_element(lost_packets_numbers->begin(),lost_packets_numbers->end()));
        dup_ACKS++;
        EV << "dup ack is " << dup_ACKS << " min packet loss is: " << min_packet_lost << " recovery start packet is " << recovery_start_packet  <<   endl;
        if ((dup_ACKS >= 3 && min_packet_lost < recovery_start_packet) || (min_packet_lost > recovery_start_packet)) {
            bool start_epoch = congestion_alg->receivedDuplicateAck(dup_ACKS);
            if (start_epoch) {
                recovery_start_packet = packet_counter;
                EV << "################ enter recovery ############" << endl;
            }
        }
    }

    else {
        if (largest >= recovery_start_packet) {
            // stop recovery stage
            congestion_alg->receivedDataAck();
            dup_ACKS = 1;
            EV << "################ exit recovery ############" << endl;
        }
        else {
            // inflating cwnd
             congestion_alg->inflateCwnd();
             dup_ACKS++;
        }
    }
}


void QuicConnectionClient::updateStreamInfo(Packet* copy_of_ACKED_packet) {
    // update number of ACKED bytes and free blocked streams if necessary
    cObject* p = copy_of_ACKED_packet->getObject("info");
    retransmission_info* info = dynamic_cast<retransmission_info*>(p);
    auto curr_header = copy_of_ACKED_packet->popAtFront<QuicPacketHeader>();
    b offset = b(0);
    while (offset < copy_of_ACKED_packet->getDataLength()) {
        auto curr_frame = copy_of_ACKED_packet->peekDataAt<QuicFrame>(offset);
        int type = curr_frame->getFrame_type();
        if (type == 5) { // temporary - until receiving different types of frames
            auto stream_frame = copy_of_ACKED_packet->peekDataAt<StreamFrame>(offset);
            // check if retransmission
            if (info->getIs_retransmit()) {
                // free relevant streams
                int stream_id = stream_frame->getStream_id();
                stream_arr->freeStream(stream_id);
            }
            // update ACKED bytes
            int stream_id = stream_frame->getStream_id();
            int num_of_bytes = stream_frame->getLength();
            stream_arr->updateACKedBytes(stream_id, num_of_bytes);
        }
        offset += curr_frame->getChunkLength();
    }
}


void QuicConnectionClient::processRexmitTimer(){
    congestion_alg->processRexmitTimer();
}


std::list<Packet*>* QuicConnectionClient::getPacketsToSend() {
    return packets_to_send;
}


void QuicConnectionClient::updateLostPackets(int largest) {
    std::vector<int>* lost_packets_numbers = send_queue->updateLostPackets(largest);
    updateCongestionAlgo(lost_packets_numbers,largest);
}



std::list<Packet*>* QuicConnectionClient::getLostPackets() {
    std::list<Packet*>* lost_packets = send_queue->getLostPackets();
    for (std::list<Packet*>::iterator it =
            lost_packets->begin(); it != lost_packets->end(); ++it) {
            createRetransmitPacket(*it, false);
    }
    return lost_packets;
}


int QuicConnectionClient::getSendWindow() {
    return std::min(connection_flow_control_recieve_window, congestion_alg->getSndCwnd());
}


int QuicConnectionClient::getNumBytesSent() {
    return num_bytes_sent;
}


int QuicConnectionClient::getNumBytesSentWithRet() {
    return bytes_sent_with_ret;
}


int QuicConnectionClient::getCurrentBytesSent(bool with_ret) {
    int bytes_sent;
    if (with_ret) {
        bytes_sent = current_sent_bytes_with_ret;
        current_sent_bytes_with_ret = 0;
    }
    else {
        bytes_sent = current_sent_bytes;
        current_sent_bytes = 0;
    }
    return bytes_sent;
}


simtime_t QuicConnectionClient::getRto(){
    return congestion_alg->getRto();
}


simtime_t QuicConnectionClient::getRtt(){
    return congestion_alg->getRtt();
}


bool QuicConnectionClient::getReconnect(){
    return reconnect;
}


int QuicConnectionClient::getConnectionWindow() {
    return connection_flow_control_recieve_window;
}


int QuicConnectionClient::getMaxPayload() {
    return max_payload;
}


int QuicConnectionClient::getStreamWindow() {
    return stream_flow_control_window;
}


int QuicConnectionClient::getStreamNumber(){
    return stream_arr->getStreamNumber();
}


int QuicConnectionClient::getStreamBytesSent(int stream_id) {
    return stream_arr->getStreamBytesSent(stream_id);
}


void QuicConnectionClient::setStreamNumber(int new_stream_number){
    stream_arr->setStreamNumber(new_stream_number);
}


Packet* QuicConnectionClient::findPacketInSendQueue(int packet_number) {
    return send_queue->findPacketInSendQueue(packet_number);
}

std::list<Packet*>* QuicConnectionClient::getPacketsToCancel(int cancel_RTO_before_number) {
    return send_queue->getPacketsToCancel(cancel_RTO_before_number);
}


Packet* QuicConnectionClient::findInitialPacket() {
    return send_queue->findInitialPacket();
}


bool QuicConnectionClient::getEndConnection() {
    return stream_arr->getAllStreamsDone();
}


} /* namespace inet */


