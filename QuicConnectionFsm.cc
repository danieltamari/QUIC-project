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

namespace inet {

bool QuicConnection::ProcessEvent(const QuicEventCode& event, cMessage *msg) {
    switch (event) {
    case QUIC_E_INIT:
        ProcessInitState(msg);
        break;
        case QUIC_E_NEW_CONNECTION:
        ProcessNewConnection(msg); // add function
        break;

    case QUIC_E_RECONNECTION:
        ProcessReconnection(msg); //add function
        break;

    case QUIC_E_CONNECTION_EST:
        ProcessConnectionEst(msg); //add function
        break;
    case QUIC_E_CONNECTION_TERM:
        processConnectionTerm(msg);
        break;

    default:
        //  throw cRuntimeError(tcpMain, "wrong event code");
        break;
    }

    // then state transitions
        return performStateTransition(event);
}

bool QuicConnection::performStateTransition(const QuicEventCode& event) {
    int oldState = fsm.getState();

    switch (fsm.getState()) {
    case QUIC_S_INIT:
        switch (event) {
        case QUIC_E_NEW_CONNECTION:
            FSM_Goto(fsm, QUIC_S_NEW_CONNECTION);
            break;
        case QUIC_E_RECONNECTION:
            FSM_Goto(fsm, QUIC_S_RECONNECTION);
            break;
        default:
            break;
        }
        break;

    case QUIC_S_NEW_CONNECTION:
        switch (event) {
        case QUIC_E_CONNECTION_EST:
            FSM_Goto(fsm, QUIC_S_CONNECTION_EST);
            break;
        case QUIC_E_CONNECTION_TERM:
            FSM_Goto(fsm, QUIC_S_CONNECTION_TERM);
            break;
        default:
            break;
        }
        break;

    case QUIC_S_RECONNECTION:
        switch (event) {
        case QUIC_E_CONNECTION_EST:
            FSM_Goto(fsm, QUIC_S_CONNECTION_EST);
            break;
        case QUIC_E_CONNECTION_TERM:
            FSM_Goto(fsm, QUIC_S_CONNECTION_TERM);
            break;
    default:
        break;
        }
        break;

    case QUIC_S_CONNECTION_EST:
        switch (event) {
        case QUIC_E_CONNECTION_TERM:
            FSM_Goto(fsm, QUIC_S_CONNECTION_TERM);
            break;
        default:
            break;
        }
        break;

    case QUIC_S_CONNECTION_TERM:
        break;
    }

    // #### this is a way in tcp to print state names and events, maybe adjust it for debugging.

    /*
     if (oldState != fsm.getState()) {
     EV_INFO << "Transition: " << stateName(oldState) << " --> " << stateName(fsm.getState()) << "  (event was: " << eventName(event) << ")\n";
     //EV_DEBUG_C("testing") << tcpMain->getName() << ": " << stateName(oldState) << " --> " << stateName(fsm.getState()) << "  (on " << eventName(event) << ")\n";
     }
     else {
     EV_DETAIL << "Staying in state: " << stateName(fsm.getState()) << " (event was: " << eventName(event) << ")\n";
     }
     */
    return fsm.getState() != QUIC_S_CONNECTION_TERM;
}

} /* namespace inet */
