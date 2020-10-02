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

#include "SimV1_client.h"

#include <omnetpp.h>
#include <string>

namespace inet {

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3


//#define SENDER 1
//#define RECEIVER 2

SimV1_client::SimV1_client() {
    //my_connection = new QuicConnection(8, 0, 0, 000);
}

SimV1_client::~SimV1_client() {
    // TODO Auto-generated destructor stub
}

void SimV1_client::initialize(int stage) {
    //EV << "initialize simv1_client" << endl;
    ApplicationBase::initialize(stage);
    this->first_connection = true;
    //this->my_connection->SetToFirstConnection();
    // need to add connection id parameter and update it here

    this->tOpen=0;
    this->tSend=par("tSend");
    //this->total_bytes_to_send=TOTAL_BYTES;


    fsm_state = new cMessage("fsm_state");
    //scheduleAt(simTime(), fsm_state);

}

void SimV1_client::handleMessageWhenUp(cMessage *msg) {
    EV << "handle message when up in simv1 client" << endl;
    fsmClient(msg);
}

void SimV1_client::handleStartOperation(LifecycleOperation *operation) {
    EV << "handleStartOperation in simV1 client" << endl;
    if (simTime() <= tOpen) {
        fsm_state->setKind(MSGKIND_CONNECT);
        scheduleAt(tOpen, fsm_state);
    }
}

void SimV1_client::fsmClient(cMessage *msg) {
    switch (msg->getKind()) {
    case MSGKIND_CONNECT:
        //  if (activeOpen)
        connect(); // sending will be scheduled from socketEstablished()
        //    else
        //       throw cRuntimeError("TODO");
        break;

    case MSGKIND_SEND:
        sendData();
        break;

    case MSGKIND_CLOSE:
        close();
        break;

    default:
        throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}

void SimV1_client::connect() {
// Todo in the future add paramaters of connection or something.
    fsm_state->setKind(MSGKIND_SEND);
    scheduleAt(std::max(tSend, simTime()), fsm_state);

}

void SimV1_client::sendData() {
    //int number_of_streams = par("number_of_streams");
    //int stream_size=par("stream_size");

    std::string streams = par("streams_list");
    std::vector<int> streams_vect;

    std::stringstream ss(streams);

    for (int i; ss >> i;) {
        streams_vect.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }

    //int server_number_to_send=par("server_number_to_send");
    //int my_client_number=par("my_client_number");

    const char *connectAddress = par("connectAddress");


    Packet *msg = new Packet("connection_data");
    const auto &payload = makeShared<connection_config_data>();
    payload->setConnection_dataArraySize(streams_vect.size());
    for (std::size_t i = 0; i < streams_vect.size(); i++){
         payload->setConnection_data(i, streams_vect[i]);
    }
    payload->setConnectAddress(connectAddress);
    //payload->setMy_client_number(my_client_number);
    payload->setChunkLength(B(sizeof(int)*1));
    msg->insertAtBack(payload);
    int kind = par("sim_type");
    msg->setKind(kind);
    //long numBytes=this->total_bytes_to_send;
    //EV_INFO << "sending array size: "<< number_of_streams << "with stream_size " << stream_size;
    send(msg, "out");
//    if (++commandIndex < (int)commands.size()) {
//        simtime_t tSend = commands[commandIndex].tSend;
//        scheduleAt(std::max(tSend, simTime()), timeoutMsg);
//    }
//    else {
    fsm_state->setKind(MSGKIND_CLOSE);
    scheduleAt(std::max(tClose, simTime()), fsm_state);
}



Packet *SimV1_client::createDataPacket(long sendBytes)
{
    // in the tcp session app there are 3 ways to send bytes maybe we need to add i too.
    // for now lets say we only send bytes.
    Ptr<Chunk> payload;
    payload = makeShared<ByteCountChunk>(B(sendBytes));
    //payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    Packet *packet = new Packet("data1");
    packet->insertAtBack(payload);
    return packet;
}


void SimV1_client::close(){
    //TODO add this function.
}

void SimV1_client::handleStopOperation(LifecycleOperation *operation) {
    EV << "stop OPERATIONNNNNN" << endl;
}
void SimV1_client::handleCrashOperation(LifecycleOperation *operation) {
    EV << "crash OPERATIONNNNNN" << endl;

}

} /* namespace inet */
