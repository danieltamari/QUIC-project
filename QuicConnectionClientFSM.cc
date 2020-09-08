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

#include "QuicConnectionClient.h"

namespace inet {

Packet* QuicConnectionClient::ProcessEvent(const QuicEventCode &event,
        Packet *packet) {
    Packet *conn_packet = NULL;
    switch (event) {
    case QUIC_E_CLIENT_INITIATE_HANDSHAKE:
        conn_packet = ProcessInitiateHandshake(packet);
        break;

    case QUIC_E_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE:
        conn_packet = ProcessClientHandshakeResponse(packet); // add function
        break;

    case QUIC_E_SEND:
        conn_packet = ProcessClientSend(packet); //add function
        break;
    case QUIC_E_CLIENT_PROCESS_ACK:
        conn_packet = ProcessClientACK(packet);
        break;

    default:
        //  throw cRuntimeError(tcpMain, "wrong event code");
        break;
    }

    // then state transitions
    performStateTransition(event);
    return conn_packet;
}

void QuicConnectionClient::performStateTransition(const QuicEventCode &event) {
    switch (fsm.getState()) {
    case QUIC_S_CLIENT_INITIATE_HANDSHAKE:
        switch (event) {
        case QUIC_E_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE:
            FSM_Goto(fsm, QUIC_S_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE);
            break;
        default:
            break;
        }
        break;

    case QUIC_S_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE:
        switch (event) {
        case QUIC_E_CLIENT_PROCESS_ACK:
            FSM_Goto(fsm, QUIC_S_CLIENT_PROCESS_ACK);
            break;
            //case QUIC_E_CONNECTION_TERM:
            //   FSM_Goto(fsm, QUIC_S_CONNECTION_TERM);
            //   break;
        default:
            break;
        }
        break;

    case QUIC_S_SEND:
        switch (event) {
        case QUIC_E_CLIENT_PROCESS_ACK:
            FSM_Goto(fsm, QUIC_S_CLIENT_PROCESS_ACK);
            break;
        default:
            break;
        }

    case QUIC_S_CLIENT_PROCESS_ACK:
        switch (event) {
        case QUIC_E_SEND:
            FSM_Goto(fsm, QUIC_S_SEND);
            break;
        default:
            break;
        }

        break;

    }

    return;
}

} /* namespace inet */
