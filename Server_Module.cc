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

#include "Server_Module.h"

#include <omnetpp.h>

namespace inet {

#define MSGKIND_CONNECT        1
#define MSGKIND_CLOSE      2
#define SENDER 1
#define RECEIVER 2


Server_Module::Server_Module() {

}

Server_Module::~Server_Module() {
    // TODO Auto-generated destructor stub
}

void Server_Module::initialize(int stage) {
    ApplicationBase::initialize(stage);
    this->tOpen = 0;
    fsm_state = new cMessage("fsm_state");
}

void Server_Module::handleMessageWhenUp(cMessage *msg) {
    EV << "handle message when up in simv1 server" << endl;
    fsmServer(msg);
}

void Server_Module::handleStartOperation(LifecycleOperation *operation) {
    EV << "handleStartOperation in simV1 server" << endl;
    if (simTime() <= tOpen) {
        fsm_state->setKind(MSGKIND_CONNECT);
        scheduleAt(tOpen, fsm_state);
    }
}

void Server_Module::fsmServer(cMessage *msg) {
    switch (msg->getKind()) {
    case MSGKIND_CONNECT:
        connect();
        break;

    case MSGKIND_CLOSE:
        close();
        break;

    default:
        throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}

void Server_Module::connect() {
    if (tClose >= SIMTIME_ZERO) {
        fsm_state->setKind(MSGKIND_CLOSE);
        scheduleAt(tClose, fsm_state);
        Packet *msg = new Packet("initial server");
        msg->setKind(RECEIVER);
        send(msg, "out");

    }
}

void Server_Module::close(){
    //TODO add this function.
}

void Server_Module::handleStopOperation(LifecycleOperation *operation) {
    EV << "stop OPERATIONNNNNN" << endl;
}
void Server_Module::handleCrashOperation(LifecycleOperation *operation) {
    EV << "crash OPERATIONNNNNN" << endl;

}

} /* namespace inet */
