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
#define Max_Stream_ReceiveWindow 16 * 1024 * 1024   // 16 MB
#define Max_Connection_ReceiveWindow 24 * 1024 * 1024  // 24 MB
#define DEAFULT_STREAM_NUM 10
#define MAX_PAYLOAD_PACKET 2000
#define FRAMES 3

namespace inet {

QuicConnection::QuicConnection() {
    stream_arr = new QuicStreamArr();
    recieve_queue = new QuicRecieveQueue();
  //  send_queue = new QuicSendQueue();
    this->packet_counter = 0;
    this->num_packets_sent = 0;
    this->num_packets_recieved = 0;
    this->total_bytes_to_send = 0;
    // will need to configure these for socket operations
    //this->destIPaddress = IP_address;

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

QuicConnection::QuicConnection(int* connection_data, int connection_data_size) {
    stream_arr = new QuicStreamArr(connection_data_size);
    recieve_queue = new QuicRecieveQueue();
   // send_queue = new QuicSendQueue();
    //send_queue->addAllData(data_size);
    for (int i=0; i<connection_data_size; i++) {
        AddNewStream(connection_data[i],i);
    }

    this->packet_counter = 0;
    this->num_packets_sent = 0;
    this->num_packets_recieved = 0;
    this->total_bytes_to_send = 0;
    this->first_connection = true;

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
    packet_counter++;

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

StreamsData* QuicConnection::CreateSendData(int max_payload) {
    StreamsData *data_to_send = this->stream_arr->DataToSend(max_payload);
    return data_to_send;
}


void QuicConnection::recievePacket(Packet *packet) {
//    //emit(packetReceivedSignal, packet); for omnet simulation
//    num_packets_recieved++;
//
//    //### nned to add header
//    auto header = packet->popAtFront<QuicPacketHeader>(); // remove header
//
//    // extract information from header
//    int income_packet_number = header->getPacket_number();
//    int dest_connectionID  = header->getDest_connectionID();
//    int source_connectionID  = header->getSrc_connectionID();
//
//    EV << "Packet's header info: packet number is " << income_packet_number << " dest connection ID is " <<
//            dest_connectionID << " source connection ID is " << source_connectionID << endl;
//
//    int src_ID = this->GetSourceID();
//    EV << "connection src ID is " << src_ID <<  endl;
//       auto data = packet->peekData<QuicData>(); // get data from packet
//       // process data
////       StreamsData* incoming_streams_data = data->getStream_frames();
////       int num_of_frames = incoming_streams_data.getNumFrames();
////       for (int i = 0; i < num_of_frames; i++) { // ## change this after fixing header
////           int stream_id = incoming_streams_data.getStreamID(i);
////           int offset = incoming_streams_data.getOffset(i);
////           int length = incoming_streams_data.getLength(i);
////           EV << "stream_id is " << stream_id << " offset is " << offset << " length is " << length << endl;
////
////           bool is_FIN = incoming_streams_data.getFIN(i);
//           if (is_FIN)
//               recieve_queue->updateFinal(stream_id, offset, length);
//           recieve_queue->updateBuffer(stream_id, offset, length);
//           if (recieve_queue->check_if_ended(stream_id)) {
//               //  handle stream ending operations
//           }
//       }
//       delete packet;
//
//    char msgName[32];
//    sprintf(msgName, "QUIC packet ACK-%d", packet_counter++);
//    Packet *ack = new Packet(msgName);
//
//    const auto &payload = makeShared<ApplicationPacket>();
//
//    int msgByteLength = 5;
//    payload->setChunkLength(B(msgByteLength));
//    ack->insertAtBack(payload);
//
//    //L3Address destAddr = chooseDestAddr();
//    //socket.sendTo(ack, destAddr, this->destPort);
//    num_packets_sent++;
}

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

//void QuicConnection::ProcessInitialClientData(int total_bytes_to_send) {
//    this->total_bytes_to_send = total_bytes_to_send;
//    this->send_queue->addAllData(total_bytes_to_send);
//}

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
    this->connection_window = initial_connection_window;
    this->stream_arr->setAllStreamsWindows(initial_stream_window);
    this->max_payload = max_payload;

    StreamsData *send_data = this->CreateSendData(max_payload);
    Packet* packet_to_send = createQuicDataPacket(send_data);
    this->event->setKind(QUIC_S_SEND);
    return packet_to_send;
}

Packet* QuicConnection::ProcessServerWaitData(Packet* packet) {

}




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

int QuicConnection::GetEventKind() {
    return this->event->getKind();
}

//void QuicConnection::initialize() {
////    if (this->first_connection) {
////        this->event->setKind(QUIC_E_NEW_CONNECTION);
////    } else {
////        this->event->setKind(QUIC_E_RECONNECTION);
////    }
//}

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
