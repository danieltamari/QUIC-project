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

#include "SimV1server.h"

namespace inet {

SimV1_server::SimV1_server() {
    my_connection = new QuicConnection(8, 0, 0, 1000);
}

SimV1_server::~SimV1_server() {
    // TODO Auto-generated destructor stub
}

void SimV1_server::initialize() {
//    EV << "STARTTTTTTT QUIC HEREEEEEEEEEEEEEEE" << endl;
//    this->my_connection->SetToFirstConnection();
//    start_fsm = new cMessage("start_fsm");
//    scheduleAt(simTime(), start_fsm);
}

void SimV1_server::handleMessage(cMessage *msg) {
       EV << "handle message server" << endl;
//    QuicEventCode event = this->my_connection->preanalyseAppCommandEvent(
//            my_connection->GetEventKind());
//    if (event==QUIC_E_NEW_CONNECTION){
//        scheduleAt(simTime()+exponential(5),msg);
//    }
//    bool ret = my_connection->ProcessEvent(event, msg);
//    if (!ret)
//        my_connection->processConnectionTerm(msg);
}

} /* namespace inet */
