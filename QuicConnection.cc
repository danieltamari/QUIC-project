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

#define DEAFULT_STREAM_NUM 10
#define DATA_SIZE 2000
#define FRAMES 3

//for init state to know which connection are we.
#define SENDER 1
#define RECEIVER 2

namespace inet {

QuicConnection::QuicConnection() {
    stream_arr = new QuicStreamArr(DEAFULT_STREAM_NUM);
    recieve_queue = new QuicRecieveQueue();
    send_queue = new QuicSendQueue();
    this->packet_counter = 0;
    this->num_packets_sent = 0;
    this->num_packets_recieved = 0;
    this->total_bytes_to_send = 0;
    // will need to configure these for socket operations
    this->localPort = -1;
    this->destPort = -1;
    //this->destIPaddress = IP_address;

    this->first_connection = true;
    char fsmname[24];
    //sprintf(fsmname, "fsm-%d", socketId); ### CHANGE DO THIS LATER WHEN CONFIGUE SOCKET ID
    sprintf(fsmname, "quic_fsm");
    fsm.setName(fsmname);
    fsm.setState(QUIC_S_INIT);
    this->event = new cMessage("INITAL");

    //JUST FOR THE SIM
    this->curr_data_size = DATA_SIZE;
    this->curr_frames_number = FRAMES;
}
QuicConnection::QuicConnection(int max_streams_num, int localPort, int destPort,
        int total_bytes/*, L3Address IP_address*/) {
    stream_arr = new QuicStreamArr(max_streams_num);
    recieve_queue = new QuicRecieveQueue();
    send_queue = new QuicSendQueue();
    //send_queue->addAllData(total_bytes);

    this->packet_counter = 0;
    this->num_packets_sent = 0;
    this->num_packets_recieved = 0;
    this->total_bytes_to_send = total_bytes;

    // will need to configure these for socket operations
    //this->localPort = localPort;
    //this->destPort = destPort;

    //this->destIPaddress = IP_address;

    //const char *addrModeStr = par("chooseDestAddrMode");
    //int addrMode = cEnum::get("inet::ChooseDestAddrMode")->lookup(addrModeStr);
    //if (addrMode == -1)
    //    throw cRuntimeError("Invalid chooseDestAddrMode: '%s'", addrModeStr);
    //chooseDestAddrMode = static_cast<ChooseDestAddrMode>(addrMode);

    this->first_connection = true;
    char fsmname[24];
    //sprintf(fsmname, "fsm-%d", socketId); ### CHANGE DO THIS LATER WHEN CONFIGUE SOCKET ID
    sprintf(fsmname, "quic_fsm");
    fsm.setName(fsmname);
    fsm.setState(QUIC_S_INIT);
    this->event = new cMessage("INITAL");

}

QuicConnection::~QuicConnection() {
    // TODO Auto-generated destructor stub
}

Packet* QuicConnection::createQuicPacket(const StreamsData sterams_data) {
    char msgName[32];
    sprintf(msgName, "QUIC packet number-%d", packet_counter++);

    // long msgByteLength = 30; //need to change this just for ane exaple to run sim!!!!!!!!!!

    Packet *packet = new Packet(msgName);
    //auto QuicHeader = makeShared<QuicPacketHeader>();
    // need to set header's dest & source connection ID & setChunkLength &
    //QuicHeader->setPacket_number(packet_counter);
    // packet->insertAtFront(QuicHeader);
    const auto &payload = makeShared<QuicData>();
    //int msgByteLength = sterams_data.getTotalSize();
    int msgByteLength = 200;
    //send_queue->removeDataSent(msgByteLength); // update the send queue
    payload->setChunkLength(B(msgByteLength));

    payload->setStream_frames(sterams_data);
    packet->insertAtBack(payload);
    return packet;
}

void QuicConnection::sendPacket(Packet *packet) {
    L3Address destAddr = chooseDestAddr();
    // emit(packetSentSignal, packet); for omnet simulation
    socket.sendTo(packet, destAddr, this->destPort);

    num_packets_sent++;
    //send (packet,"toc_out");//only for sim
}

StreamsData* QuicConnection::CreateSendData(int frames_number,
        int bytes_to_send_in_packet) {
    StreamsData *data_to_send = this->stream_arr->DataToSend(frames_number,
            bytes_to_send_in_packet);
    return data_to_send;
}

void QuicConnection::recievePacket(Packet *packet) {
    //emit(packetReceivedSignal, packet); for omnet simulation
    num_packets_recieved++;

    //### nned to add header
    //auto header = packet->popAtFront<QuicPacketHeader>(); // remove header

    // extract information from header
    //int income_packet_numer = header->getPacket_number();
    //int number_of_frames = header->getNum_of_frames();

    auto data = packet->peekData<QuicData>(); // get data from packet
    // process data
    const StreamsData incoming_frames = data->getStream_frames();
    for (int i = 0; i < 2; i++) { // ## change this after fixing header
        int stream_id = incoming_frames.getStreamID(i);
        int offset = incoming_frames.getOffset(i);
        int length = incoming_frames.getLength(i);
        EV << "stream_id is " << stream_id << " offset is " << offset << " length is " << length << endl;

        bool is_FIN = incoming_frames.getFIN(i);
        if (is_FIN)
            recieve_queue->updateFinal(stream_id, offset, length);
        recieve_queue->updateBuffer(stream_id, offset, length);
        if (recieve_queue->check_if_ended(stream_id)) {
            //  handle stream ending operations
        }
    }
    delete packet;

    char msgName[32];
    sprintf(msgName, "QUIC packet ACK-%d", packet_counter++);
    Packet *ack = new Packet(msgName);

    const auto& payload = makeShared<ApplicationPacket>();

    int msgByteLength = 5;
    payload->setChunkLength(B(msgByteLength));
    ack->insertAtBack(payload);

    L3Address destAddr = chooseDestAddr();
    socket.sendTo(ack, destAddr, this->destPort);
    num_packets_sent++;
}

void QuicConnection::socketDataArrived(UdpSocket *socket, Packet *packet) {
    EV << "data arrived everything is ok" << endl;

    QuicEventCode event = preanalyseAppCommandEvent(this->event->getKind());
    if (event == QUIC_E_CONNECTION_EST) {
        EV << "got ack in client" << endl;
        bool ret = ProcessEvent(event, packet);
        if (!ret)
            processConnectionTerm(packet);
        return;
    }
    // process incoming packet
    recievePacket(packet);
}

void QuicConnection::socketErrorArrived(UdpSocket *socket,
        Indication *indication) {
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void QuicConnection::socketClosed(UdpSocket *socket) {
    //if (operationalState == State::STOPPING_OPERATION)
    //    startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

L3Address QuicConnection::chooseDestAddr()
{
    if (destAddresses.size() == 1)
        return destAddresses[0];

    int k = getRNG(destAddrRNG)->intRand(destAddresses.size());
    return destAddresses[k];
}

int QuicConnection::AddNewStream(int max_bytes, int quanta, int total_bytes) {
    return this->stream_arr->AddNewStream(max_bytes, quanta, total_bytes);
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

void QuicConnection::ProcessInitialClientData(int total_bytes_to_send) {
    this->total_bytes_to_send = total_bytes_to_send;
    this->send_queue->addAllData(total_bytes_to_send);
}

void QuicConnection::ProcessInitState(cMessage *msg) {
    EV << "process init in quicConnection" << endl;
    if (msg->getKind() == SENDER) {
        EV << "i am a client!!" << endl;
        Packet *packet = check_and_cast<Packet*>(msg);
        uint32 total_bytes = (packet->getByteLength());
        ProcessInitialClientData(total_bytes);
    }
    if (this->first_connection)
        this->event->setKind(QUIC_E_NEW_CONNECTION);
    else
        this->event->setKind(QUIC_E_RECONNECTION);
    localPort = par("localPort");
    destPort = par("destPort");
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);

    //const char *localAddress = par("localAddress");
    //socket.bind(*localAddress ?L3AddressResolver().resolve(localAddress) : L3Address(),this->local_port);
    //setSocketOptions();

    socket.bind(localPort);

    destAddrRNG = par("destAddrRNG");

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;
    //bool excludeLocalDestAddresses = par("excludeLocalDestAddresses");

    //IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);

    while ((token = tokenizer.nextToken()) != nullptr) {
        if (strstr(token, "Broadcast") != nullptr)
            destAddresses.push_back(Ipv4Address::ALLONES_ADDRESS);
        else {
            L3Address addr = L3AddressResolver().resolve(token);
            //      if (excludeLocalDestAddresses && ift && ift->isLocalAddress(addr))
            //         continue;
            destAddresses.push_back(addr);
        }
    }
    if (msg->getKind() == RECEIVER) {
        EV << "i am a reciever!!" << endl;
        this->event->setKind(QUIC_E_CONNECTION_TERM);

    }

    //destAddr = chooseDestAddr();

//    const char *destAddrs = par("destAddresses");
//    cStringTokenizer tokenizer(destAddrs);
//    const char *token;
//
//    while ((token = tokenizer.nextToken()) != nullptr) {
//        destAddressStr.push_back(token);
//        L3Address result;
//        L3AddressResolver().tryResolve(token, result);
//        if (result.isUnspecified())
//            EV_ERROR << "cannot resolve destination address: " << token << endl;
//        destAddresses.push_back(result);
//    }

    //only for the sim #####
    this->AddNewStream(200, 100, 600);
    this->AddNewStream(300, 200, 700);
    this->AddNewStream(400, 300, 600);
    ///#########

    scheduleAt(simTime(), msg);
}

void QuicConnection::ProcessNewConnection(cMessage *msg) {
//    EV << "process new connection in quicConnection" << endl;
    scheduleAt(simTime() + exponential(5), msg);
    this->event->setKind(QUIC_E_CONNECTION_EST);
}

void QuicConnection::ProcessReconnection(cMessage *msg) {
    EV << "process REconnection in quicConnection" << endl;
    this->event->setKind(QUIC_E_CONNECTION_EST);
    scheduleAt(simTime() + exponential(3), msg);
}

void QuicConnection::ProcessConnectionEst(cMessage *msg) {
    EV << "im hereeeeee in connection established" << endl;
    char msgName[32];
    sprintf(msgName, "QuicPacket");
    Packet *send_packet = new Packet(msgName);
    int* total_bytes_sent_in_packet = 0;
    StreamsData *send_data = this->CreateSendData(curr_frames_number,
            curr_data_size,total_bytes_sent_in_packet);

    this->send_queue->removeDataSent(*total_bytes_sent_in_packet); // need to do all kinds of checks to see how much data left and everything.
    send_packet = this->createQuicPacket(*send_data);
    sendPacket(send_packet);

}

void QuicConnection::processConnectionTerm(cMessage *msg) {
    EV << "im hereeeeee in connection term" << endl;
    // TODO: add code
    // maybe just need to call destructor or something
}

//void QuicConnection::moveDataToSendQueue(int bytes_num) {
//    this->send_queue->addAllData(bytes_num);
//}

int QuicConnection::GetEventKind() {
    return this->event->getKind();
}

void QuicConnection::initialize() {
//    if (this->first_connection) {
//        this->event->setKind(QUIC_E_NEW_CONNECTION);
//    } else {
//        this->event->setKind(QUIC_E_RECONNECTION);
//    }
}

void QuicConnection::handleMessage(cMessage *msg) {
    EV << "im hereeeeee in connection handle message" << endl;
    if (msg->arrivedOn("socketIn"))
        socket.processMessage(msg);
    else {
        QuicEventCode event = preanalyseAppCommandEvent(this->event->getKind());
        bool ret = ProcessEvent(event, msg);
        if (!ret)
            processConnectionTerm(msg);
//    //send(msg, "out");
    }
}
QuicEventCode QuicConnection::preanalyseAppCommandEvent(int commandCode) {
    switch (commandCode) {
    case QUIC_E_INIT:
        return QUIC_E_INIT;

    case QUIC_E_NEW_CONNECTION:
        return QUIC_E_NEW_CONNECTION;

    case QUIC_E_RECONNECTION:
        return QUIC_E_RECONNECTION;

    case QUIC_E_CONNECTION_EST:
        return QUIC_E_CONNECTION_EST;

    case QUIC_E_CONNECTION_TERM:
        return QUIC_E_CONNECTION_TERM;

        //default:
        // throw cRuntimeError(tcpMain, "Unknown message kind in app command");
    }
}

} /* namespace inet */
