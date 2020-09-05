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

namespace inet {

Packet* QuicConnectionServer::ProcessEvent(const QuicEventCode &event,
        Packet *packet) {
    Packet *conn_packet = NULL;
    switch (event) {
    case QUIC_E_SERVER_PROCESS_HANDSHAKE:
        conn_packet = ServerProcessHandshake(packet);
        break;

    case QUIC_E_SERVER_WAIT_FOR_DATA:
        conn_packet = ProcessServerWaitData(packet);
        break;

    default:
        //  throw cRuntimeError(tcpMain, "wrong event code");
        break;
    }

    // then state transitions
    performStateTransition(event);
    return conn_packet;
}

void QuicConnectionServer::performStateTransition(const QuicEventCode &event) {

    switch (fsm.getState()) {

    case QUIC_S_SERVER_PROCESS_HANDSHAKE:
        switch (event) {
        case QUIC_E_SERVER_WAIT_FOR_DATA:
            FSM_Goto(fsm, QUIC_S_SERVER_WAIT_FOR_DATA);
            break;
        default:
            break;
        }
        break;

    case QUIC_S_SERVER_WAIT_FOR_DATA:
        switch (event) {
        //case QUIC_E_SEND://Delete later

//            FSM_Goto(fsm, QUIC_S_SEND);
//            break;
//        case QUIC_E_CONNECTION_TERM:
//            FSM_Goto(fsm, QUIC_S_CONNECTION_TERM);
        break;
    default:
        break;
        }
        break;
    }
    return;
}

} /* namespace inet */
