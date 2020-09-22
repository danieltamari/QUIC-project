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

#include <omnetpp.h>
#include "SimV1_server.h"

namespace inet {

#define MSGKIND_CONNECT        1
#define MSGKIND_CLOSE      2
#define SENDER 1
#define RECEIVER 2


SimV1_server::SimV1_server() {
    //my_connection = new QuicConnection(8, 0, 0, 1000); // #### change!!!
}

SimV1_server::~SimV1_server() {
    // TODO Auto-generated destructor stub
}

void SimV1_server::initialize(int stage) {
    //EV << "initialize simv1_server" << endl;
    ApplicationBase::initialize(stage);
    this->first_connection = true;
    //this->my_connection->SetToFirstConnection();
    // need to add connection id parameter and update it here

    this->tOpen = 0;
    fsm_state = new cMessage("fsm_state");
}

void SimV1_server::handleMessageWhenUp(cMessage *msg) {
    EV << "handle message when up in simv1 server" << endl;
    fsmServer(msg);
}

void SimV1_server::handleStartOperation(LifecycleOperation *operation) {
    EV << "handleStartOperation in simV1 server" << endl;
    if (simTime() <= tOpen) {
        fsm_state->setKind(MSGKIND_CONNECT);
        scheduleAt(tOpen, fsm_state);
    }
}

void SimV1_server::fsmServer(cMessage *msg) {
    switch (msg->getKind()) {
    case MSGKIND_CONNECT:
        //  if (activeOpen)
        connect(); // sending will be scheduled from socketEstablished()
        //    else
        //       throw cRuntimeError("TODO");
        break;

    case MSGKIND_CLOSE:
        close();
        break;

    default:
        throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}

void SimV1_server::connect() {
    if (tClose >= SIMTIME_ZERO) {
        fsm_state->setKind(MSGKIND_CLOSE);
        scheduleAt(tClose, fsm_state);
        Packet *msg = new Packet("initial server");
        msg->setKind(RECEIVER);
        send(msg, "out");

    }
}

void SimV1_server::close(){
    //TODO add this function.
}

void SimV1_server::handleStopOperation(LifecycleOperation *operation) {
    EV << "stop OPERATIONNNNNN" << endl;
}
void SimV1_server::handleCrashOperation(LifecycleOperation *operation) {
    EV << "crash OPERATIONNNNNN" << endl;

}

} /* namespace inet */
