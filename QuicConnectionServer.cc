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

  //  QuicConnection();//need to check if its ok

    stream_arr = new QuicStreamArr();
    max_stream_data_packets_ = new std::list<Packet*>();
    num_packets_recieved = 0;
    rcv_next = 0;
   // connection_max_flow_control_window_size=Init_Connection_FlowControl_Window;
   // connection_flow_control_recieve_offset=Init_Connection_FlowControl_Window;
   // connection_flow_control_recieve_window=Init_Connection_FlowControl_Window;
    this->destination=destination;
    inital_stream_window = Init_Stream_ReceiveWindow;
    is_out_of_order = false;
    rcv_in_order = 0;
    out_of_order_list_flushed=false;


    fsm.setState(QUIC_S_SERVER_PROCESS_HANDSHAKE);
    event->setKind(QUIC_E_SERVER_PROCESS_HANDSHAKE);
}

QuicConnectionServer::~QuicConnectionServer() {
    // TODO Auto-generated destructor stub
}


void QuicConnectionServer::recievePacket(std::vector<stream_frame*> accepted_frames) {

    num_packets_recieved++;
    for (std::vector<stream_frame*>::iterator it =
            accepted_frames.begin(); it != accepted_frames.end(); ++it) {
            stream_frame* curr_frame = *it;
            int stream_id = curr_frame->stream_id;
            int offset = curr_frame->offset;
            int length = curr_frame->length;
            bool is_FIN = curr_frame->is_FIN;
            EV << "stream_id is " << stream_id << " offset is " << offset << " length is " << length << endl;

            if(!stream_arr->isStreamExist(stream_id)) {
                stream_arr->AddNewStreamServer(stream_id, inital_stream_window);
            }

            // find the stream in stream_arr
            stream* curr_stream = stream_arr->getStream(stream_id);
            curr_stream->receive_queue->updateStreamInfo(offset,length,is_FIN);
            curr_stream->receive_queue->addStreamFrame(curr_frame);

           // if (offset + length > curr_stream->highest_recieved_byte_offset) {// only true if we get streams ooo so we won't update highest offset
              //  curr_stream->highest_recieved_byte_offset = offset + length;
              //  curr_stream->flow_control_recieve_window = curr_stream->flow_control_recieve_offset - curr_stream->highest_recieved_byte_offset;
           // }

         //  int consumed_bytes_size = curr_stream->receive_queue->moveDataToApp(); // remove all available data
         //  curr_stream->consumed_bytes += consumed_bytes_size;

//           if (curr_stream->flow_control_recieve_offset-curr_stream->consumed_bytes<curr_stream->max_flow_control_window_size/2){
//               // create max_stream_data packet
//               char msgName[32];
//               sprintf(msgName, "MAX STREAM DATA packet");
//               Packet *max_stream_data_packet = new Packet(msgName);
//               int src_ID = this->GetSourceID();
//               int dest_ID = this->GetDestID();
//               auto QuicHeader = makeShared<QuicPacketHeader>();
//               QuicHeader->setDest_connectionID(dest_ID);
//               QuicHeader->setSrc_connectionID(src_ID);
//               QuicHeader->setPacket_type(4);
//               QuicHeader->setChunkLength(B(sizeof(int)*4));
//               max_stream_data_packet->insertAtFront(QuicHeader);
//               const auto &payload = makeShared<MaxStreamData>();
//               payload->setStream_ID(stream_id);
//               int max_stream_offset=curr_stream->consumed_bytes+curr_stream->max_flow_control_window_size;//update to the client
//               curr_stream->flow_control_recieve_offset=curr_stream->consumed_bytes+curr_stream->max_flow_control_window_size;//self update moving flow control window
//               curr_stream->flow_control_recieve_window=curr_stream->flow_control_recieve_offset-curr_stream->highest_recieved_byte_offset;
//               payload->setMaximum_Stream_Data(max_stream_offset);
//               payload->setChunkLength(B(sizeof(int)*1));
//               max_stream_data_packet->insertAtBack(payload);
//               max_stream_data_packets_->push_back(max_stream_data_packet);
//           }

            if (curr_stream->receive_queue->check_if_ended()) {
               //  handle stream ending operations
           }
       }

}

Packet* QuicConnectionServer::ServerProcessHandshake(Packet* packet) {
    char msgName[32];
    sprintf(msgName, "QUIC HANDSHAKE RESPONSE");
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
    QuicHeader_->setLong_packet_type(2);
    QuicHeader_->setVersion(1);
    QuicHeader_->setDest_connection_ID(dest_ID);
    QuicHeader_->setDest_connection_id_length(dest_ID_len);
    QuicHeader_->setSource_connection_ID(src_ID);
    QuicHeader_->setSource_connection_id_length(src_ID_len);
    int header_length = LONG_HEADER_BASE_LENGTH + dest_ID_len + src_ID_len;
    QuicHeader_->setChunkLength(B(header_length));
    packet_to_send->insertAtFront(QuicHeader_);
    this->SetSourceID(src_ID);
    this->SetDestID(dest_ID);

    // create frames array for packet's payload
//    const auto &payload = makeShared<QuicFramesArray>();
//    const auto &handshake_data = makeShared<QuicHandShakeData>();
//    handshake_data->setInitial_max_data(Init_Connection_FlowControl_Window); // initial window per connection
//    handshake_data->setInitial_max_stream_data(Init_Stream_ReceiveWindow); // initial window per stream
//    handshake_data->setMax_udp_payload_size(QUIC_ALLOWED_PACKET_SIZE);
//    handshake_data->setOriginal_destination_connection_id(dest_ID);
//    handshake_data->setChunkLength(B(sizeof(int)*3 + sizeof(long)));
//    const auto &padding = makeShared<PaddingFrame>();
//    padding->setChunkLength(B(QUIC_ALLOWED_PACKET_SIZE) - handshake_data->getChunkLength() - QuicHeader_->getChunkLength());
//    payload->setFrames_arrayArraySize(2);
//    payload->setFrames_array(0, *handshake_data);
//    payload->setFrames_array(1, *padding);

    const auto &payload = makeShared<QuicHandShakeData>();
    payload->setInitial_max_data(Init_Connection_FlowControl_Window); // initial window per connection
    payload->setInitial_max_stream_data(Init_Stream_ReceiveWindow); // initial window per stream
    payload->setMax_udp_payload_size(QUIC_ALLOWED_PACKET_SIZE);
    payload->setChunkLength(B(sizeof(int)*3));
    payload->setChunkLength(B(QUIC_ALLOWED_PACKET_SIZE));
    packet_to_send->insertAtBack(payload);
    return packet_to_send;
}


void QuicConnectionServer::ProcessServerReceivedPacket(Packet* packet) {

    EV << " got into ProcessServerWaitData function" << endl;
    is_out_of_order = false;
    auto header = packet->popAtFront<QuicPacketHeader>();
    int income_packet_number = header->getPacket_number();
    int dest_connectionID  = header->getDest_connectionID();
    int source_connectionID  = header->getSrc_connectionID();
    // find out if packet is retransmission
    cObject* p = packet->getObject("info");
    retransmission_info* info = dynamic_cast<retransmission_info*>(p);
    if (info->getIs_retransmit()) {
        int original_packet_number = info->getOriginal_packet_number();
        if (original_packet_number < rcv_next) {
            if (income_packet_number==rcv_next){
                rcv_next++;
                rcv_in_order++;
                EV<< "unnecessary retransmit "<< endl;
                return;
            }
            else {
                receive_not_ACKED_queue.push_back(income_packet_number);
                EV<< "unnecessary retransmit "<< endl;
                return;
            }
        }
        else {
            receive_not_ACKED_queue.push_back(original_packet_number);
            receive_not_ACKED_queue.push_back(income_packet_number);
        }
    }

    else if (rcv_next != income_packet_number) { // OOO packet
        receive_not_ACKED_queue.push_back(income_packet_number);
        is_out_of_order = true;
    }

    else     {
        rcv_next++;
        rcv_in_order++;
    }

    EV << "Packet's header info: packet number is " << income_packet_number << " dest connection ID is " <<
                dest_connectionID << " source connection ID is " << source_connectionID << endl;

    auto data = packet->peekData<QuicData>(); // get data from packet
    // process data
    const StreamsData* streams_data = data->getStream_frames();
    std::vector<stream_frame*> accepted_frames = streams_data->getFramesArray();
    recievePacket(accepted_frames);

    // update flow control
   // int max_offset=this->GetMaxOffset();
   // this->connection_flow_control_recieve_window=this->connection_flow_control_recieve_offset-max_offset;

    // pop packets' numbers from Queue
//    if (receive_not_ACKED_queue.size() != 0) {
//        while (receive_not_ACKED_queue.size() != 0) {
//            int curr_number = receive_not_ACKED_queue.front();
//            if (curr_number != rcv_next) {
//               // receive_not_ACKED_queue.push_front(curr_number);
//                break;
//            }
//            receive_not_ACKED_queue.pop_front();
//            rcv_next++;
//        }
//    }


    EV << "########### receive next before: " << rcv_next <<" ###############" << endl;

    std::list<int>::iterator it = receive_not_ACKED_queue.begin();
    while (it != receive_not_ACKED_queue.end()) {
        if ((*it) <= rcv_next){
            int packet_number_to_remove = (*it);

            receive_not_ACKED_queue.remove(packet_number_to_remove);
            rcv_next++;
            rcv_in_order++;
            out_of_order_list_flushed=true;
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
    current_Ack_frame = makeShared<QuicACKFrame>();
    current_Ack_frame->setFirst_ACK_range(0);
    current_Ack_frame->setChunkLength(B(sizeof(int)*4));
}



std::list<Packet*>* QuicConnectionServer::getMaxStreamDataPackets()  {
    return max_stream_data_packets_;
}


//int QuicConnectionServer::GetTotalConsumedBytes(){
//    return stream_arr->getTotalConsumedBytes();
//}
//
//int QuicConnectionServer::GetConnectionsRecieveOffset(){
//    return connection_flow_control_recieve_offset;
//}
//
//int QuicConnectionServer::GetConnectionMaxWindow(){
//    return connection_max_flow_control_window_size;
//}

bool QuicConnectionServer::GetIsOutOfOrder() {
    return is_out_of_order;
}

IntrusivePtr<inet::QuicACKFrame> QuicConnectionServer::getCurrentAckFrame() {
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
bool  QuicConnectionServer::GetListFlushed(){
    this->out_of_order_list_flushed;
}
void  QuicConnectionServer::SetListFlushed(bool set){
    this->out_of_order_list_flushed=set;
}

} /* namespace inet */
