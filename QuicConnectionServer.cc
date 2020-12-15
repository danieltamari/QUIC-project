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

#include "QuicConnectionServer.h"
#include <omnetpp.h>

namespace inet {


QuicConnectionServer::QuicConnectionServer() {
    // TODO Auto-generated constructor stub

}


QuicConnectionServer::QuicConnectionServer(L3Address destination_, int init_stream_flow_control_window) {
    ended = false;
    stream_array = new QuicStreamArr();
    receive_queue = new QuicReceiveQueue();
    num_packets_recieved = 0;
    rcv_next = 0;
    destination = destination_;
    inital_stream_window = init_stream_flow_control_window;
    current_largest=-1;
    num_streams = 0;
    num_streams_ended = 0;
    current_rcvd_bytes = 0;
    current_rcvd_bytes_with_ret = 0;
    current_rcvd_bytes_long = 0;
    current_rcvd_bytes_with_ret_long = 0;
}


QuicConnectionServer::~QuicConnectionServer() {
    // TODO Auto-generated destructor stub
}


Packet* QuicConnectionServer::ServerProcessHandshake(Packet* packet,int max_payload, int init_stream_flow_control_window,int init_connection_flow_control_winodw) {
    char msgName[32];
    sprintf(msgName, "QUIC INITIAL PACKET (RESPONSE)");
    int src_ID = getSourceID();
    int dest_ID = getDestID();
    // get connections IDs length
    unsigned int dest_ID_len = calcSizeInBytes(dest_ID);
    unsigned int src_ID_len = calcSizeInBytes(src_ID);
    // create HANDSHAKE packet
    Packet *packet_to_send = new Packet(msgName);
    // create long header
    auto QuicHeader_ = makeShared<QuicLongHeader>();
    QuicHeader_->setHeader_form(b(1));
    QuicHeader_->setFixed_bit(b(1));
    QuicHeader_->setLong_packet_type(0);
    QuicHeader_->setVersion(1);
    QuicHeader_->setDest_connection_ID(dest_ID);
    QuicHeader_->setDest_connection_id_length(dest_ID_len);
    QuicHeader_->setSource_connection_ID(src_ID);
    QuicHeader_->setSource_connection_id_length(src_ID_len);
    int header_length = LONG_HEADER_BASE_LENGTH + dest_ID_len + src_ID_len;
    QuicHeader_->setChunkLength(B(header_length));
    packet_to_send->insertAtFront(QuicHeader_);

    // create frames for packet's payload
    const auto &handshake_data = makeShared<QuicHandShakeData>();
    handshake_data->setInitial_max_data(init_connection_flow_control_winodw); // initial window per connection
    handshake_data->setInitial_max_stream_data(init_stream_flow_control_window); // initial window per stream
    handshake_data->setMax_udp_payload_size(max_payload);
    handshake_data->setOriginal_destination_connection_id(dest_ID);
    handshake_data->setChunkLength(B(sizeof(int)*3 + sizeof(long)));
    packet_to_send->insertAtBack(handshake_data);
    const auto &padding = makeShared<PaddingFrame>();
    padding->setChunkLength(B(QUIC_MIN_PACKET_SIZE) - handshake_data->getChunkLength() - QuicHeader_->getChunkLength());
    packet_to_send->insertAtBack(padding);
    delete packet;
    return packet_to_send;
}


bool QuicConnectionServer::ProcessServerReceivedPacket(Packet* packet) {
    this->num_packets_recieved++;
    int packet_size = packet->getByteLength();
    auto header = packet->peekAtFront<QuicPacketHeader>();
    int income_packet_number = header->getPacket_number();
    int dest_connectionID  = header->getDest_connection_ID();
    // find out if packet is retransmission
    cObject* p = packet->getObject("info");
    retransmission_info* info = dynamic_cast<retransmission_info*>(p);
    bool new_data = true;
    if (info->getIs_retransmit()) {
        // go over all the history of retransmissions
        std::vector<int> original_packets = info->getPackets_numbers();
        for (std::vector<int>::iterator it =
                original_packets.begin(); it != original_packets.end(); ++it) {
            int original_packet_number = *it;
            if (original_packet_number < rcv_next) { // unnecessary retransmission
                new_data = false;
                received_out_of_order.push_back(income_packet_number);

            }
            else { // add to receive_not_ACKED_queue all the previous numbers of this packet
                received_out_of_order.push_back(original_packet_number);
                received_out_of_order.push_back(income_packet_number);
            }
        }
    }

    else if (rcv_next != income_packet_number) { // out of order packet
        received_out_of_order.push_back(income_packet_number);
    }

    else     { // in order packet
        rcv_next++;
    }

    EV << "Packet's header info: packet number is " << income_packet_number << " dest connection ID is " <<
                dest_connectionID << endl;

    received_out_of_order.sort();
    received_out_of_order.unique();

    EV << "########### receive next before: " << rcv_next <<" ###############" << endl;

    // pop packets' numbers from Queue
    std::list<int>::iterator it = received_out_of_order.begin();
    while (it != received_out_of_order.end()) {
        if ((*it) <= rcv_next){
            int packet_number_to_remove = (*it);
            received_out_of_order.remove(packet_number_to_remove);
            rcv_next++;
            it = received_out_of_order.begin();
        }
        else {
            it++;
        }
    }

    // print receive_not_ACKED_queue:
    EV << "########### receive_not_ACKED_queue: ###############" << endl;
    for (std::list<int>::iterator it =
            received_out_of_order.begin(); it != received_out_of_order.end(); ++it) {
        EV << "packet number: " << *it << endl;
    }

    EV << "########### receive next after: " << rcv_next <<" ###############" << endl;
    current_rcvd_bytes_with_ret += packet_size;
    current_rcvd_bytes_with_ret_long += packet_size;
    if (new_data) {
        current_rcvd_bytes += packet_size;
        current_rcvd_bytes_long += packet_size;
    }
    return new_data;
}


void QuicConnectionServer::ProcessStreamDataFrame(inet::Ptr<const StreamFrame> stream_frame) {
    int stream_id = stream_frame->getStream_id();
    int offset = stream_frame->getOffset();
    int length = stream_frame->getLength();
    bool is_FIN = stream_frame->getIs_FIN();
    EV << "connection id of server: " << this->connection_source_ID << endl;
    EV << "connection id of client: " << this->connection_dest_ID << endl;
    EV << "stream_id is " << stream_id << " offset is " << offset << " length is " << length << endl;
    // add new stream at server's side if not already exists
    if(!stream_array->isStreamExist(stream_id)) {
        stream_array->addNewStreamServer(stream_id, inital_stream_window);
        num_streams++;
    }

    // update new accepted bytes in streams info
    receive_queue->updateStreamStatus(stream_id,offset,length,is_FIN);

    // check if stream has ended
    if (receive_queue->checkIfEnded(stream_id)) {
       //  handle stream ending operations
       stream_array->closeStream(stream_id);
       receive_queue->removeStreamStatus(stream_id);
       EV << "******** stream " << stream_id << " ended at server *********" << endl;
       num_streams_ended++;
       if (num_streams_ended == num_streams) {
           ended = true;
       }
   }
}


int QuicConnectionServer::getCurrentBytesReceived(bool with_ret) {
    int bytes_rcvd;
    if (with_ret) {
        bytes_rcvd = current_rcvd_bytes_with_ret;
        current_rcvd_bytes_with_ret = 0;
    }
    else {
        bytes_rcvd = current_rcvd_bytes;
        current_rcvd_bytes = 0;
    }
    return bytes_rcvd;
}


int QuicConnectionServer::getCurrentBytesReceivedLong(bool with_ret) {
    int bytes_rcvd;
    if (with_ret) {
        bytes_rcvd = current_rcvd_bytes_with_ret_long;
        current_rcvd_bytes_with_ret_long = 0;
    }
    else {
        bytes_rcvd = current_rcvd_bytes_long;
        current_rcvd_bytes_long = 0;
    }
    return bytes_rcvd;
}


int QuicConnectionServer::getRcvNext(){
    return rcv_next;
}


int QuicConnectionServer::getCurrLargest(){
    return current_largest;
}


std::list<int> QuicConnectionServer::getNotAckedList(){
    return received_out_of_order;
}


int QuicConnectionServer::getLargestInOrder(){
    return rcv_next-1;
}


void QuicConnectionServer::setCurrLargest(int largest){
    current_largest=largest;
}


void QuicConnectionServer::setLargestWithRcvNext(){
    current_largest=rcv_next-1;;
}

int QuicConnectionServer::getRcvdPackets() {
    return num_packets_recieved;
}


bool QuicConnectionServer::getEndConnection() {
    return ended;
}


} /* namespace inet */
