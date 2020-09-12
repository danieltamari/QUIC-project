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



#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "QuicStreamArr.h"
#include "QuicData_m.h"
#include "QuicPacketHeader_m.h"
#include "QuicHandShakeData_m.h"
#include "MaxStreamData_m.h"
#include "StreamsData.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"

#ifndef INET_APPLICATIONS_QUICAPP_QUICCONNECTION_H_
#define INET_APPLICATIONS_QUICAPP_QUICCONNECTION_H_

namespace inet {

enum QuicState {
    QUIC_S_CLIENT_INITIATE_HANDSHAKE = 0,
    QUIC_S_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE = FSM_Steady(1),
    QUIC_S_CLIENT_PROCESS_ACK = FSM_Steady(2),
    QUIC_S_SEND = FSM_Steady(3),
    QUIC_S_SERVER_PROCESS_HANDSHAKE = FSM_Steady(4),
    QUIC_S_SERVER_WAIT_FOR_DATA =FSM_Steady(5),
//    QUIC_S_CONNECTION_TERM = FSM_Steady(8),
};

enum QuicEventCode {
    QUIC_E_CLIENT_INITIATE_HANDSHAKE = 0,
    QUIC_E_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE,
    QUIC_E_CLIENT_PROCESS_ACK,
    QUIC_E_SEND,
    QUIC_E_SERVER_PROCESS_HANDSHAKE,
    QUIC_E_SERVER_WAIT_FOR_DATA,
//    QUIC_E_CONNECTION_TERM,
};


class QuicConnection {
public:
    QuicConnection();
    virtual ~QuicConnection();

    int GetSourceID();
    void SetSourceID(int source_ID);
    int GetDestID();
    void SetDestID(int dest_ID);
    int GetMaxOffset();
    L3Address GetDestAddress();
    void setConnectionsRecieveOffset(int offset);
    void setConnectionsRecieveWindow(int window_size);

    //virtual void performStateTransition(const QuicEventCode &event) = 0;
    //virtual Packet* ProcessEvent(const QuicEventCode &event,Packet* packet) = 0;

    //Packet* ActivateFsm(Packet* packet);
    //QuicEventCode preanalyseAppCommandEvent(int commandCode);

protected:
    int connection_source_ID;
    int connection_dest_ID;
    L3Address destination;

    QuicStreamArr *stream_arr;

    cFSM fsm; // QUIC state machine
    cMessage *event = nullptr;
    cMessage *start_fsm;


    // both server and client flow control side parameters, only server update them
    int connection_max_flow_control_window_size; // constant
    int connection_flow_control_recieve_offset; //
    int connection_flow_control_recieve_window; // flow_control_recieve_offset - highest_recieved_byte_offset

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICCONNECTION_H_ */
