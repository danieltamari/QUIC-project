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
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"


namespace inet {

QuicConnectionServer::QuicConnectionServer() {
    // TODO Auto-generated constructor stub

}

QuicConnectionServer::QuicConnectionServer(L3Address destination) {
    stream_arr = new QuicStreamArr();
    max_stream_data_packets_ = new std::list<Packet*>();
    num_packets_recieved = 0;
    rcv_next = 0;
    this->destination=destination;
    inital_stream_window = Init_Stream_ReceiveWindow;
    is_out_of_order = false;
    rcv_in_order = 0;
}


QuicConnectionServer::~QuicConnectionServer() {
    // TODO Auto-generated destructor stub
}

void QuicConnectionServer::ProcessStreamDataFrame(inet::Ptr<const StreamFrame> stream_frame) {
    int stream_id = stream_frame->getStream_id();
    int offset = stream_frame->getOffset();
    int length = stream_frame->getLength();
    bool is_FIN = stream_frame->getIs_FIN();
    EV << "stream_id is " << stream_id << " offset is " << offset << " length is " << length << endl;
    // add new stream at server's side if not already exists
    if(!stream_arr->isStreamExist(stream_id)) {
        stream_arr->AddNewStreamServer(stream_id, inital_stream_window);
    }
    // find the stream in stream_arr
    stream* curr_stream = stream_arr->getStream(stream_id);
    curr_stream->receive_queue->updateStreamInfo(offset,length,is_FIN);

    // ### CHECK if moveDataToApp is needed ?
 //  int consumed_bytes_size = curr_stream->receive_queue->moveDataToApp(); // remove all available data

    if (curr_stream->receive_queue->check_if_ended()) {
       //  handle stream ending operations
   }
}


void QuicConnectionServer::recievePacket(Packet* accepted_packet) {
    // process data
    b offset_in_packet = b(0);
    while (offset_in_packet < accepted_packet->getDataLength()) {
        auto curr_frame = accepted_packet->peekDataAt<QuicFrame>(offset_in_packet);
     //   offset_in_packet += curr_frame->getChunkLength();
       // auto curr_frame = accepted_packet->peekAtFront<QuicFrame>();
        int type = curr_frame->getFrame_type();
        if (type == 8) { // temporary - until receiving different types of frames
            auto stream_frame = accepted_packet->peekDataAt<StreamFrame>(offset_in_packet);
           // offset_in_packet += stream_frame->getLength();
            int stream_id = stream_frame->getStream_id();
            int offset = stream_frame->getOffset();
            int length = stream_frame->getLength();
            bool is_FIN = stream_frame->getIs_FIN();
            EV << "stream_id is " << stream_id << " offset is " << offset << " length is " << length << endl;
            // add new stream at server's side if not already exists
            if(!stream_arr->isStreamExist(stream_id)) {
                stream_arr->AddNewStreamServer(stream_id, inital_stream_window);
            }
            // find the stream in stream_arr
            stream* curr_stream = stream_arr->getStream(stream_id);
            curr_stream->receive_queue->updateStreamInfo(offset,length,is_FIN);

            // ### CHECK if moveDataToApp is needed ?
         //  int consumed_bytes_size = curr_stream->receive_queue->moveDataToApp(); // remove all available data

            if (curr_stream->receive_queue->check_if_ended()) {
               //  handle stream ending operations
           }
        }
        offset_in_packet += curr_frame->getChunkLength();
    }


}

Packet* QuicConnectionServer::ServerProcessHandshake(Packet* packet) {
    char msgName[32];
    sprintf(msgName, "QUIC INITIAL PACKET (RESPONSE)");
    int src_ID = this->GetSourceID();
    int dest_ID = this->GetDestID();
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
    handshake_data->setInitial_max_data(Init_Connection_FlowControl_Window); // initial window per connection
    handshake_data->setInitial_max_stream_data(Init_Stream_ReceiveWindow); // initial window per stream
    handshake_data->setMax_udp_payload_size(QUIC_ALLOWED_PACKET_SIZE);
    handshake_data->setOriginal_destination_connection_id(dest_ID);
    handshake_data->setChunkLength(B(sizeof(int)*3 + sizeof(long)));
    packet_to_send->insertAtBack(handshake_data);
    const auto &padding = makeShared<PaddingFrame>();
    padding->setChunkLength(B(QUIC_ALLOWED_PACKET_SIZE) - handshake_data->getChunkLength() - QuicHeader_->getChunkLength());
    packet_to_send->insertAtBack(padding);
    delete packet;
    return packet_to_send;
}


void QuicConnectionServer::ProcessServerReceivedPacket(Packet* packet) {

    EV << " got into ProcessServerWaitData function" << endl;
    is_out_of_order = false;
    bool income_in = false;
    //auto header = packet->popAtFront<QuicPacketHeader>();
    auto header = packet->peekAtFront<QuicPacketHeader>();
    int income_packet_number = header->getPacket_number();
    int dest_connectionID  = header->getDest_connection_ID();
    // find out if packet is retransmission
    cObject* p = packet->getObject("info");
    retransmission_info* info = dynamic_cast<retransmission_info*>(p);
    if (info->getIs_retransmit()) {
        // go over all the history of retransmissions
        std::vector<int> original_packets = info->getPackets_numbers();
        for (std::vector<int>::iterator it =
                original_packets.begin(); it != original_packets.end(); ++it) {
            int original_packet_number = *it;
            if (original_packet_number < rcv_next) { // unnecessary retransmission
                if (income_packet_number == rcv_next) {
                    rcv_next++;
                    rcv_in_order++;
                    return;
                }
                else {
                    receive_not_ACKED_queue.push_back(income_packet_number);
                    return;
                }
            }
            else { // add to receive_not_ACKED_queue all the previous numbers of this packet
                receive_not_ACKED_queue.push_back(original_packet_number);
                if (!income_in) {
                    receive_not_ACKED_queue.push_back(income_packet_number);
                    income_in = true;
                }
            }
        }
    }

    else if (rcv_next != income_packet_number) { // out of order packet
        receive_not_ACKED_queue.push_back(income_packet_number);
        is_out_of_order = true;
    }

    else     { // in order packet
        rcv_next++;
        rcv_in_order++;
    }

    EV << "Packet's header info: packet number is " << income_packet_number << " dest connection ID is " <<
                dest_connectionID << endl;

    // process received frames in packet
  //  recievePacket(packet);

    receive_not_ACKED_queue.sort();

    EV << "########### receive next before: " << rcv_next <<" ###############" << endl;

    // pop packets' numbers from Queue
    std::list<int>::iterator it = receive_not_ACKED_queue.begin();
    while (it != receive_not_ACKED_queue.end()) {
        if ((*it) <= rcv_next){
            int packet_number_to_remove = (*it);

            receive_not_ACKED_queue.remove(packet_number_to_remove);
            rcv_next++;
            rcv_in_order++;
            it = receive_not_ACKED_queue.begin();
        }
        else {
            it++;
        }
    }

    // print receive_not_ACKED_queue:
    EV << "########### receive_not_ACKED_queue: ###############" << endl;
    for (std::list<int>::iterator it =
            receive_not_ACKED_queue.begin(); it != receive_not_ACKED_queue.end(); ++it) {
        EV << "packet number: " << *it << endl;
    }

    EV << "########### receive next after: " << rcv_next <<" ###############" << endl;

}



int QuicConnectionServer::GetRcvNext(){
    return rcv_next;
}


void QuicConnectionServer::createAckFrame() {
    // create ACK frame
    current_Ack_frame = makeShared<ACKFrame>();
    current_Ack_frame->setFirst_ACK_range(0);
    current_Ack_frame->setFrame_type(2);
    current_Ack_frame->setChunkLength(B(sizeof(int)*4));
}


std::list<Packet*>* QuicConnectionServer::getMaxStreamDataPackets()  {
    return max_stream_data_packets_;
}


bool QuicConnectionServer::GetIsOutOfOrder() {
    return is_out_of_order;
}

IntrusivePtr<inet::ACKFrame> QuicConnectionServer::getCurrentAckFrame() {
    return current_Ack_frame;
}

std::list<int> QuicConnectionServer::GetNotAckedList(){
    return this->receive_not_ACKED_queue;
}

int QuicConnectionServer::getLargestInOrder(){
    return rcv_next-1;
}

int QuicConnectionServer::GetRcvInOrderAndRst(){
    int rcv_in_order_=this->rcv_in_order;
    rcv_in_order=0;
    return rcv_in_order_;
}

void QuicConnectionServer:: RcvInOrdedRst(){
    rcv_in_order=0;
}


} /* namespace inet */
