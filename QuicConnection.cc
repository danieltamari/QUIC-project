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

#include "QuicConnection.h"
#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"

#define Min_FlowControl_Window 16 * 1024  // 16 KB
#define Init_Connection_FlowControl_Window 64 * 1024  // 16 KB

#define Max_Stream_ReceiveWindow 16 * 1024 * 1024   // 16 MB
#define Max_Connection_ReceiveWindow 24 * 1024 * 1024  // 24 MB
#define DEAFULT_STREAM_NUM 10
#define MAX_PAYLOAD_PACKET 2000
#define FRAMES 3


namespace inet {

//constructor of connection in the server
QuicConnection::QuicConnection() {
    stream_arr = new QuicStreamArr();
  //  recieve_queue = new QuicRecieveQueue();
  //  send_queue = new QuicSendQueue();
    this->packet_counter = 0;
    this->num_packets_sent = 0;
    this->num_packets_recieved = 0;
    this->total_bytes_to_send = 0;
    //this->total_consumed_bytes =0;
    //this->total_flow_control_recieve_offset=0;
    connection_max_flow_control_window_size=Init_Connection_FlowControl_Window;
    connection_flow_control_recieve_offset=Init_Connection_FlowControl_Window;
    connection_flow_control_recieve_window=Init_Connection_FlowControl_Window;

    // will need to configure these for socket operations
    //this->destIPaddress = IP_address;
    this->inital_stream_window = Min_FlowControl_Window;
    this->first_connection = true;
    char fsmname[24];
    //sprintf(fsmname, "fsm-%d", socketId); ### CHANGE DO THIS LATER WHEN CONFIGUE SOCKET ID
    sprintf(fsmname, "quic_fsm");
    fsm.setName(fsmname);
    fsm.setState(QUIC_S_SERVER_PROCESS_HANDSHAKE);
    this->event = new cMessage("INITAL");
    event->setKind(QUIC_E_SERVER_PROCESS_HANDSHAKE);


//    //JUST FOR THE SIM
//    this->curr_data_size = DATA_SIZE;
//    this->curr_frames_number = FRAMES;
}

QuicConnection::QuicConnection(int* connection_data, int connection_data_size,int server_number_to_send) {
    stream_arr = new QuicStreamArr(connection_data_size);
   // recieve_queue = new QuicRecieveQueue();
    //send_queue = new QuicConnectionSendQueue();

    //send_queue->addAllData(data_size);

    for (int i=0; i<connection_data_size; i++) {
        AddNewStream(connection_data[i],i);
    }
    //this->total_consumed_bytes =0;
    //this->total_flow_control_recieve_offset=0;
    this->packet_counter = 0;
    this->num_packets_sent = 0;
    this->num_packets_recieved = 0;
    this->total_bytes_to_send = 0;
    this->first_connection = true;
    this->inital_stream_window = Min_FlowControl_Window;
    this->module_number_to_send=server_number_to_send;

    char fsmname[24];
    //sprintf(fsmname, "fsm-%d", socketId); ### CHANGE DO THIS LATER WHEN CONFIGUE SOCKET ID
    sprintf(fsmname, "quic_fsm");
    fsm.setName(fsmname);
    fsm.setState(QUIC_S_CLIENT_INITIATE_HANDSHAKE);
    this->event = new cMessage("INITAL");
    event->setKind(QUIC_E_CLIENT_INITIATE_HANDSHAKE);


    //JUST FOR THE SIM
   // this->curr_data_size = DATA_SIZE;
   // this->curr_frames_number = FRAMES;
}


QuicConnection::~QuicConnection() {
    // TODO Auto-generated destructor stub
}

void QuicConnection::setClientNumber(int client_number){
    this->module_number_to_send=client_number;
}

int QuicConnection::GetClientNumber(){
    return module_number_to_send;
}

Packet* QuicConnection::createQuicDataPacket(StreamsData* streams_data) {
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
    //int msgByteLength = 200;
    //send_queue->removeDataSent(msgByteLength); // update the send queue
    payload->setChunkLength(B(msgByteLength));
    payload->setStream_frames(streams_data);
    packet_to_send->insertAtBack(payload);
    return packet_to_send;
}

void QuicConnection::sendPacket(Packet *packet) {
    //L3Address destAddr = chooseDestAddr();
    // emit(packetSentSignal, packet); for omnet simulation
    //socket.sendTo(packet, destAddr, this->destPort);

    num_packets_sent++;
    //send (packet,"toc_out");//only for sim
}

StreamsData* QuicConnection::CreateSendData(int max_payload, int connection_window) {
    StreamsData *data_to_send = this->stream_arr->DataToSend(max_payload, connection_window);
    return data_to_send;
}


void QuicConnection::recievePacket(std::vector<stream_frame*> accepted_frames) {

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
          //  EV << "curr_stream ID " << curr_stream->receive_queue->getStreamID() << " curr_stream final size " << curr_stream->receive_queue->getfinal_size() <<
             //       " curr_stream last received " << curr_stream->receive_queue->getlast_frame_received() << endl;

            curr_stream->receive_queue->addStreamFrame(curr_frame);

            if (offset + length > curr_stream->highest_recieved_byte_offset) {// if only true if we get streams ooo so we won't update highest offset
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
               max_stream_data_packets_.push_back(max_stream_data_packet);
           }

            if (curr_stream->receive_queue->check_if_ended()) {
//               //  handle stream ending operations
           }
       }
//    this->total_consumed_bytes = stream_arr->getTotalConsumedBytes();
//    this->total_flow_control_recieve_offset = stream_arr->getTotalMaxOffset();
//    if (total_flow_control_recieve_offset-total_consumed_bytes<connection_flow_control_recieve_window/2){
//        send_max_data_packet=true;
//    }

}

std::vector<Packet*> QuicConnection::getMaxStreamDataPackets()  {
    return max_stream_data_packets_;
}

void QuicConnection::updateMaxStreamData(int stream_id, int max_stream_data_offset){
    stream* curr_stream = stream_arr->getStream(stream_id);
    curr_stream->flow_control_recieve_offset=max_stream_data_offset;
    curr_stream->flow_control_recieve_window=curr_stream->flow_control_recieve_offset-curr_stream->highest_recieved_byte_offset;
}



void QuicConnection::AddNewStream(int stream_size, int index) {
    this->stream_arr->AddNewStream(stream_size, index);
}

bool QuicConnection::CloseStream(int stream_id) {
    return this->stream_arr->CloseStream(stream_id);
}

int QuicConnection::GetDataSize() {
    return this->curr_data_size;
}

void QuicConnection::SetDataSize(int data_size) {
    this->curr_data_size = data_size;
}

int QuicConnection::GetFramesNumber() {
    return this->curr_frames_number;
}

void QuicConnection::SetFramesNumber(int frames_number) {
    this->curr_frames_number = frames_number;
}

int QuicConnection::GetSourceID() {
    return this->connection_source_ID;
}

void QuicConnection::SetSourceID(int source_ID) {
    this->connection_source_ID = source_ID;
}

int QuicConnection::GetDestID() {
    return this->connection_dest_ID;
}

void QuicConnection::SetDestID(int dest_ID) {
    this->connection_dest_ID = dest_ID;
}


Packet* QuicConnection::ProcessInitiateHandshake(Packet* packet) {
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
    QuicHeader->setClient_number(packet->getKind());//###### NEED TO CHANGE

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


Packet* QuicConnection::ServerProcessHandshake(Packet* packet) {

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
    int msgByteLength = 1;
    payload->setInitial_max_data(Min_FlowControl_Window); // initial window per connection
    payload->setInitial_max_stream_data(Min_FlowControl_Window); // initial window per stream
    payload->setMax_udp_payload_size(MAX_PAYLOAD_PACKET);
    //payload->setMax_udp_payload_size(); find value for UDP payload
    payload->setChunkLength(B(sizeof(int)*3));
    packet_to_send->insertAtBack(payload);
    this->event->setKind(QUIC_S_SERVER_WAIT_FOR_DATA);
    return packet_to_send;
}

Packet* QuicConnection::ProcessClientHandshakeResponse(Packet* packet) {

    auto header = packet->popAtFront<QuicPacketHeader>();
    auto data = packet->peekData<QuicHandShakeData>(); // get data from packet
    // process data
    int initial_stream_window = data->getInitial_max_stream_data();
    int initial_connection_window = data->getInitial_max_data();
    int max_payload = data->getMax_udp_payload_size();
    this->connection_max_flow_control_window_size = initial_connection_window;
    this->connection_flow_control_recieve_offset = initial_connection_window;
    this->connection_flow_control_recieve_window = initial_connection_window;

    this->stream_arr->setAllStreamsWindows(initial_stream_window);
    this->max_payload = max_payload;

    StreamsData *send_data = this->CreateSendData(max_payload, connection_flow_control_recieve_window);
//    int num_bytes_to_send = send_data->getTotalSize();
//    connection_flow_control_recieve_window -= num_bytes_to_send;
    Packet* packet_to_send = createQuicDataPacket(send_data);
    Packet* copy_packet_to_send=createQuicDataPacket(send_data);
    packet_counter++;
    this->send_queue.push_back(copy_packet_to_send);

    num_packets_sent++;
    this->event->setKind(QUIC_S_SEND);
    return packet_to_send;
}



Packet* QuicConnection::ProcessServerWaitData(Packet* packet) {

    EV << " got into ProcessServerWaitData function" << endl;

    auto header = packet->popAtFront<QuicPacketHeader>();
    // extract information from header
    int income_packet_number = header->getPacket_number();
    int dest_connectionID  = header->getDest_connectionID();
    int source_connectionID  = header->getSrc_connectionID();

    EV << "Packet's header info: packet number is " << income_packet_number << " dest connection ID is " <<
                dest_connectionID << " source connection ID is " << source_connectionID << endl;

    auto data = packet->peekData<QuicData>(); // get data from packet
    // process data
    const StreamsData* streams_data = data->getStream_frames();
    std::vector<stream_frame*> accepted_frames = streams_data->getFramesArray();
  //  int received_data_size = streams_data->getTotalSize();
    recievePacket(accepted_frames);

    // update flow control
    this->connection_flow_control_recieve_window=this->connection_flow_control_recieve_offset-this->GetMaxOffset();
    //int total_connection_consumed_bytes=it->GetTotalConsumedBytes();
    //int connection_window_size=it->GetWindowSize();
    //int max_data=total_connection_consumed_bytes+connection_window_size; // check if correct

    // create ACK packet
    char msgName[32];
    sprintf(msgName, "QUIC packet ACK-%d", income_packet_number);
    Packet *ack = new Packet(msgName);
    int src_ID = this->GetSourceID();
    int dest_ID = this->GetDestID();
    auto QuicHeader = makeShared<QuicPacketHeader>();
    QuicHeader->setPacket_number(income_packet_number);
    QuicHeader->setSrc_connectionID(src_ID);
    QuicHeader->setDest_connectionID(dest_ID);
    QuicHeader->setPacket_type(3);
    QuicHeader->setChunkLength(B(sizeof(int)*4));
    ack->insertAtFront(QuicHeader);
    //this->event->setKind(QUIC_S_SEND);
    return ack;
}

Packet* QuicConnection::ProcessClientSend(Packet* packet){
    StreamsData *send_data = this->CreateSendData(max_payload, connection_flow_control_recieve_window);
    Packet* packet_to_send = createQuicDataPacket(send_data);
    Packet* copy_packet_to_send=createQuicDataPacket(send_data);
    this->send_queue.push_back(copy_packet_to_send);
    packet_counter++;
    num_packets_sent++;
    return packet_to_send;
}

Packet* QuicConnection::RemovePacketFromQueue(int packet_number){
    Packet* packet_to_remove;
    for (std::list<Packet*>::iterator it =
            send_queue.begin(); it != send_queue.end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number()==packet_number){
                packet_to_remove=*it;
                send_queue.remove(packet_to_remove);
                break;
            }
    }
    return packet_to_remove;
}

void QuicConnection::updateFlowControl(Packet* acked_packet){
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


int QuicConnection::GetEventKind() {
    return this->event->getKind();
}


int QuicConnection::GetTotalConsumedBytes(){
    return this->stream_arr->getTotalConsumedBytes();
}

int QuicConnection::GetConnectionsRecieveWindow(){
    return this->connection_flow_control_recieve_window;
}

int QuicConnection::GetMaxOffset(){
    return this->stream_arr->getTotalMaxOffset();
}


int QuicConnection::GetConnectionsRecieveOffset(){
    return this->connection_flow_control_recieve_offset;
}

int QuicConnection::GetConnectionMaxWindow(){
    return this->connection_max_flow_control_window_size;
}

void QuicConnection::setConnectionsRecieveOffset(int offset){
    this->connection_flow_control_recieve_offset = offset;
}

void QuicConnection::setConnectionsRecieveWindow(int window_size) {
    this->connection_flow_control_recieve_window = window_size;
}

Packet* QuicConnection::ActivateFsm(Packet* packet) {
    EV << "im hereeeeee in connection ActivateFsm" << endl;
        QuicEventCode event = preanalyseAppCommandEvent(this->event->getKind());
        Packet* ret_packet = ProcessEvent(event,packet);
        return ret_packet;
;

}
QuicEventCode QuicConnection::preanalyseAppCommandEvent(int commandCode) {
    switch (commandCode) {
    case QUIC_E_CLIENT_INITIATE_HANDSHAKE:
        return QUIC_E_CLIENT_INITIATE_HANDSHAKE;

    case  QUIC_E_SERVER_PROCESS_HANDSHAKE:
        return QUIC_E_SERVER_PROCESS_HANDSHAKE;

    case QUIC_E_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE:
        return QUIC_E_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE;

    case QUIC_E_SERVER_WAIT_FOR_DATA:
        return QUIC_E_SERVER_WAIT_FOR_DATA;

    case QUIC_E_RECONNECTION:
        return QUIC_E_RECONNECTION;

    case QUIC_E_SEND:
        return QUIC_E_SEND;

    case QUIC_E_LISTEN:
        return QUIC_E_LISTEN;

    case QUIC_E_CONNECTION_TERM:
        return QUIC_E_CONNECTION_TERM;

        //default:
        // throw cRuntimeError(tcpMain, "Unknown message kind in app command");
    }
}

} /* namespace inet */


//############## DELETE THIS:

//void QuicConnection::initialize() {
////    if (this->first_connection) {
////        this->event->setKind(QUIC_E_NEW_CONNECTION);
////    } else {
////        this->event->setKind(QUIC_E_RECONNECTION);
////    }
//}

//void QuicConnection::ProcessNewConnection(cMessage *msg) {
////    EV << "process new connection in quicConnection" << endl;
//    //scheduleAt(simTime() + exponential(5), msg);
//    this->event->setKind(QUIC_E_SEND);
//}
//
//void QuicConnection::ProcessReconnection(cMessage *msg) {
//    EV << "process REconnection in quicConnection" << endl;
//    this->event->setKind(QUIC_E_SEND);
//    //scheduleAt(simTime() + exponential(3), msg);
//}
//
//Packet* QuicConnection::ProcessConnectionSend(cMessage *msg) {
//    char msgName[32];
//    sprintf(msgName, "QuicPacket");
//    Packet *send_packet = new Packet(msgName);
//    if (this->send_queue->isQueueEmpty()){
//        EV << "no more data to send" << endl;
//        this->event->setKind(QUIC_E_CONNECTION_TERM);
//        //scheduleAt(simTime(),msg); //return to the fsm to finish connection
//        return send_packet;
//    }
//    EV << "im hereeeeee in connection send" << endl;
//
//    StreamsData *send_data = this->CreateSendData(curr_data_size);
//    int total_bytes_sent_in_packet = send_data->getTotalSize();
//
//    this->send_queue->removeDataSent(total_bytes_sent_in_packet); // need to do all kinds of checks to see how much data left and everything.
//    send_packet = this->createQuicPacket(*send_data);
//    //sendPacket(send_packet);
//    return send_packet;
//
//}
//void QuicConnection::ProcessConnectionListen(cMessage *msg) {
//    EV << "im hereeeeee in connection listen" << endl;
//    recievePacket((Packet*)msg);
//}
//
//void QuicConnection::processConnectionTerm(cMessage *msg) {
//    EV << "im hereeeeee in connection term" << endl;
//    // TODO: add code
//    // maybe just need to call destructor or something
//}

//void QuicConnection::moveDataToSendQueue(int bytes_num) {
//    this->send_queue->addAllData(bytes_num);
//}

//void QuicConnection::ProcessInitialClientData(int total_bytes_to_send) {
//    this->total_bytes_to_send = total_bytes_to_send;
//    this->send_queue->addAllData(total_bytes_to_send);
//}

//void QuicConnection::socketDataArrived(UdpSocket *socket, Packet *packet) {
//    EV << "data arrived everything is ok" << endl;
//    QuicEventCode event = preanalyseAppCommandEvent(this->event->getKind());
//    bool ret = ProcessEvent(event, packet);
//    if (!ret)
//        processConnectionTerm(packet);
//    return;
//
//    // process incoming packet
//}

//void QuicConnection::socketErrorArrived(UdpSocket *socket,
//        Indication *indication) {
//    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
//    delete indication;
//}
//
//void QuicConnection::socketClosed(UdpSocket *socket) {
//    //if (operationalState == State::STOPPING_OPERATION)
//    //    startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
//}
//
//L3Address QuicConnection::chooseDestAddr() {
//    if (destAddresses.size() == 1)
//        return destAddresses[0];
//
//    int k = getRNG(destAddrRNG)->intRand(destAddresses.size());
//    return destAddresses[k];
//}
