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
#include "headers_and_frames/QuicData_m.h"
#include "headers_and_frames/QuicPacketHeader_m.h"
#include "headers_and_frames/QuicLongHeader_m.h"
#include "headers_and_frames/QuicShortHeader_m.h"
#include "headers_and_frames/QuicHandShakeData_m.h"
#include "headers_and_frames/QuicFrame_m.h"
#include "headers_and_frames/PaddingFrame_m.h"
#include "headers_and_frames/ACKFrame_m.h"
#include "StreamsData.h"
#include "retransmission_info_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "stdlib.h"

#ifndef INET_APPLICATIONS_QUICAPP_QUICCONNECTION_H_
#define INET_APPLICATIONS_QUICAPP_QUICCONNECTION_H_


#define DEAFULT_STREAM_NUM 10
// flow control parameters
//#define Init_Connection_FlowControl_Window 16 * 1024  // 16 KB
#define Init_Connection_FlowControl_Window 1472 * 14
#define Init_Stream_ReceiveWindow 1472 * 14
// congestion control parameters
#define ACKTHRESH 3
#define MAX_THRESHOLD 0xFFFFFFFF
#define kPacketThreshold 3
// packets and headers sizes
#define QUIC_MIN_PACKET_SIZE 1200 // from RFC 14.1
#define QUIC_MAX_PACKET_SIZE 1472
#define LONG_HEADER_BASE_LENGTH 11
#define SHORT_HEADER_BASE_LENGTH 2



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

struct range {
    int gap;
    int ACK_range_length;
};

struct packet_rcv_type {
    int packet_number;
    bool in_order;
};



class QuicConnection {
public:
    QuicConnection();
    virtual ~QuicConnection();

    int GetSourceID();
    void SetSourceID(int source_ID);
    int GetDestID();
    void SetDestID(int dest_ID);
    //int GetMaxOffset();
    L3Address GetDestAddress();
    unsigned int calcSizeInBytes(int number);
    int calcTotalSize(std::vector<IntrusivePtr<StreamFrame>>* frames_to_send);
    int calcHeaderSize(bool short_header);


protected:
    int connection_source_ID;
    int connection_dest_ID;
    L3Address destination;

    QuicStreamArr *stream_arr;

    cFSM fsm; // QUIC state machine
    cMessage *event = nullptr;
    cMessage *start_fsm;

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICCONNECTION_H_ */
