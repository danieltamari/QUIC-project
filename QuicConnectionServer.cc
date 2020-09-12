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

#define Min_FlowControl_Window 16 * 1024  // 16 KB
#define Init_Connection_FlowControl_Window 64 * 1024  // 64 KB

#define Max_Stream_ReceiveWindow 16 * 1024 * 1024   // 16 MB
#define Max_Connection_ReceiveWindow 24 * 1024 * 1024  // 24 MB
#define DEAFULT_STREAM_NUM 10
#define MAX_PAYLOAD_PACKET 2000
#define ACKTHRESH 3
#define SSTHRESH_CHANGE_THIS 10

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
    connection_max_flow_control_window_size=Init_Connection_FlowControl_Window;
    connection_flow_control_recieve_offset=Init_Connection_FlowControl_Window;
    connection_flow_control_recieve_window=Init_Connection_FlowControl_Window;
    this->destination=destination;
    inital_stream_window = Min_FlowControl_Window;
    is_out_of_order = false;


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

            if (offset + length > curr_stream->highest_recieved_byte_offset) {// only true if we get streams ooo so we won't update highest offset
                curr_stream->highest_recieved_byte_offset = offset + length;
                curr_stream->flow_control_recieve_window = curr_stream->flow_control_recieve_offset - curr_stream->highest_recieved_byte_offset;
            }

           int consumed_bytes_size = curr_stream->receive_queue->moveDataToApp(); // remove all available data
           curr_stream->consumed_bytes += consumed_bytes_size;

           if (curr_stream->flow_control_recieve_offset-curr_stream->consumed_bytes<curr_stream->max_flow_control_window_size/2){
               // create max_stream_data packet
               char msgName[32];
               sprintf(msgName, "MAX STREAM DATA packet");
               Packet *max_stream_data_packet = new Packet(msgName);
               int src_ID = this->GetSourceID();
               int dest_ID = this->GetDestID();
               auto QuicHeader = makeShared<QuicPacketHeader>();
               QuicHeader->setDest_connectionID(dest_ID);
               QuicHeader->setSrc_connectionID(src_ID);
               QuicHeader->setPacket_type(4);
               QuicHeader->setChunkLength(B(sizeof(int)*4));
               max_stream_data_packet->insertAtFront(QuicHeader);
               const auto &payload = makeShared<MaxStreamData>();
               payload->setStream_ID(stream_id);
               int max_stream_offset=curr_stream->consumed_bytes+curr_stream->max_flow_control_window_size;//update to the client
               curr_stream->flow_control_recieve_offset=curr_stream->consumed_bytes+curr_stream->max_flow_control_window_size;//self update moving flow control window
               curr_stream->flow_control_recieve_window=curr_stream->flow_control_recieve_offset-curr_stream->highest_recieved_byte_offset;
               payload->setMaximum_Stream_Data(max_stream_offset);
               payload->setChunkLength(B(sizeof(int)*1));
               max_stream_data_packet->insertAtBack(payload);
               max_stream_data_packets_->push_back(max_stream_data_packet);
           }

            if (curr_stream->receive_queue->check_if_ended()) {
               //  handle stream ending operations
           }
       }

}

Packet* QuicConnectionServer::ServerProcessHandshake(Packet* packet) {
    EV << " ProcessInitiateHandshake server in quicConnection" << endl;
    char msgName[32];
    sprintf(msgName, "QUIC HANDSHAKE RESPONSE");
    int src_ID = this->GetSourceID();
    int dest_ID = this->GetDestID();
    Packet *packet_to_send = new Packet(msgName);
    auto QuicHeader = makeShared<QuicPacketHeader>();
    QuicHeader->setPacket_number(0);
    QuicHeader->setSrc_connectionID(src_ID);
    QuicHeader->setDest_connectionID(dest_ID);
    QuicHeader->setPacket_type(1);
    QuicHeader->setChunkLength(B(sizeof(int)*4));
    packet_to_send->insertAtFront(QuicHeader);

    const auto &payload = makeShared<QuicHandShakeData>();
    payload->setInitial_max_data(Init_Connection_FlowControl_Window); // initial window per connection
    payload->setInitial_max_stream_data(Min_FlowControl_Window); // initial window per stream
    payload->setMax_udp_payload_size(MAX_PAYLOAD_PACKET);
    payload->setChunkLength(B(sizeof(int)*3));
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
    if (rcv_next != income_packet_number) { // OOO packet
        receive_not_ACKED_queue.push_back(income_packet_number);
        is_out_of_order = true;
    }
    else
        rcv_next++;

    EV << "Packet's header info: packet number is " << income_packet_number << " dest connection ID is " <<
                dest_connectionID << " source connection ID is " << source_connectionID << endl;

    auto data = packet->peekData<QuicData>(); // get data from packet
    // process data
    const StreamsData* streams_data = data->getStream_frames();
    std::vector<stream_frame*> accepted_frames = streams_data->getFramesArray();
    recievePacket(accepted_frames);

    // update flow control
    int max_offset=this->GetMaxOffset();
    this->connection_flow_control_recieve_window=this->connection_flow_control_recieve_offset-max_offset;

    // pop packets' numbers from Queue
    if (receive_not_ACKED_queue.size() != 0) {
        while (receive_not_ACKED_queue.size() != 0) {
            int curr_number = receive_not_ACKED_queue.front();
            if (curr_number != rcv_next) {
               // receive_not_ACKED_queue.push_front(curr_number);
                break;
            }
            receive_not_ACKED_queue.pop_front();
            rcv_next++;
        }
    }
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


int QuicConnectionServer::GetTotalConsumedBytes(){
    return stream_arr->getTotalConsumedBytes();
}

int QuicConnectionServer::GetConnectionsRecieveOffset(){
    return connection_flow_control_recieve_offset;
}

int QuicConnectionServer::GetConnectionMaxWindow(){
    return connection_max_flow_control_window_size;
}

bool QuicConnectionServer::GetIsOutOfOrder() {
    return is_out_of_order;
}

IntrusivePtr<inet::QuicACKFrame> QuicConnectionServer::getCurrentAckFrame() {
    return current_Ack_frame;
}

} /* namespace inet */
