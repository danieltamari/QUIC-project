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

QuicConnection::QuicConnection() {
    char fsmname[24];
    sprintf(fsmname, "quic_fsm");
    fsm.setName(fsmname);
    this->event = new cMessage("INITAL");
}

QuicConnection::~QuicConnection() {
    // TODO Auto-generated destructor stub
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


int QuicConnection::GetMaxOffset(){
    return this->stream_arr->getTotalMaxOffset();
}

L3Address QuicConnection::GetDestAddress(){
    return destination;
}

void QuicConnection::setConnectionsRecieveOffset(int offset){
    this->connection_flow_control_recieve_offset = offset;
}

void QuicConnection::setConnectionsRecieveWindow(int window_size) {
    this->connection_flow_control_recieve_window = window_size;
}


Packet* QuicConnection::ActivateFsm(Packet* packet) {
    EV << "im hereeeeee in connection ActivateFsm" << endl;
        QuicEventCode event = preanalyseAppCommandEvent(this->event->getKind());
        Packet* ret_packet = ProcessEvent(event,packet);
        return ret_packet;
}


QuicEventCode QuicConnection::preanalyseAppCommandEvent(int commandCode) {
    switch (commandCode) {
    case QUIC_E_CLIENT_INITIATE_HANDSHAKE:
        return QUIC_E_CLIENT_INITIATE_HANDSHAKE;

    case QUIC_E_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE:
        return QUIC_E_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE;

    case QUIC_E_SEND:
        return QUIC_E_SEND;

    case  QUIC_E_SERVER_PROCESS_HANDSHAKE:
        return QUIC_E_SERVER_PROCESS_HANDSHAKE;

    case QUIC_E_SERVER_WAIT_FOR_DATA:
        return QUIC_E_SERVER_WAIT_FOR_DATA;

        //default:
        // throw cRuntimeError(tcpMain, "Unknown message kind in app command");
    }
}


} /* namespace inet */
