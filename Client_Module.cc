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

#include "Client_Module.h"

#include <omnetpp.h>
#include <string>

namespace inet {

#define MSGKIND_CONNECT    1
#define MSGKIND_SEND       2
#define MSGKIND_CLOSE      3


Client_Module::Client_Module() {
}

Client_Module::~Client_Module() {
    // TODO Auto-generated destructor stub
}

void Client_Module::initialize(int stage) {
    ApplicationBase::initialize(stage);

    this->tOpen=0;
    this->tSend=par("tSend");
    fsm_state = new cMessage("fsm_state");

}

void Client_Module::handleMessageWhenUp(cMessage *msg) {
    EV << "handle message when up in simv1 client" << endl;
    fsmClient(msg);
}

void Client_Module::handleStartOperation(LifecycleOperation *operation) {
    EV << "handleStartOperation in simV1 client" << endl;
    if (simTime() <= tOpen) {
        fsm_state->setKind(MSGKIND_CONNECT);
        scheduleAt(tOpen, fsm_state);
    }
}

void Client_Module::fsmClient(cMessage *msg) {
    switch (msg->getKind()) {
    case MSGKIND_CONNECT:
        connect();
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

void Client_Module::connect() {
    fsm_state->setKind(MSGKIND_SEND);
    scheduleAt(std::max(tSend, simTime()), fsm_state);

}

void Client_Module::sendData() {

    std::string streams = par("streams_list");
    std::vector<int> streams_vect;
    std::stringstream ss(streams);

    for (int i; ss >> i;) {
        streams_vect.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }


    const char *connectAddress = par("connectAddress");


    Packet *msg = new Packet("connection_data");
    const auto &payload = makeShared<connection_config_data>();
    payload->setConnection_dataArraySize(streams_vect.size());
    for (std::size_t i = 0; i < streams_vect.size(); i++){
         payload->setConnection_data(i, streams_vect[i]);
    }
    payload->setConnectAddress(connectAddress);
    payload->setChunkLength(B(sizeof(int)*1));
    msg->insertAtBack(payload);
    int kind = par("sim_type");
    msg->setKind(kind);
    send(msg, "out");
    fsm_state->setKind(MSGKIND_CLOSE);
    scheduleAt(std::max(tClose, simTime()), fsm_state);
}


void Client_Module::close(){
    EV << "module closed" << endl;
}

void Client_Module::handleStopOperation(LifecycleOperation *operation) {
    EV << "stop OPERATIONNNNNN" << endl;
}
void Client_Module::handleCrashOperation(LifecycleOperation *operation) {
    EV << "crash OPERATIONNNNNN" << endl;

}

} /* namespace inet */
