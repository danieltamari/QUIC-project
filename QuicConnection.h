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
#include "headers_and_frames/QuicPacketHeader_m.h"
#include "headers_and_frames/QuicLongHeader_m.h"
#include "headers_and_frames/QuicShortHeader_m.h"
#include "headers_and_frames/QuicHandShakeData_m.h"
#include "headers_and_frames/QuicFrame_m.h"
#include "headers_and_frames/PaddingFrame_m.h"
#include "headers_and_frames/ACKFrame_m.h"
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


// flow control parameters
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

    int getSourceID();
    void setSourceID(int source_ID);
    int getDestID();
    void setDestID(int dest_ID);
    L3Address getDestAddress();
    unsigned int calcSizeInBytes(int number);
    int calcTotalSize(std::vector<IntrusivePtr<StreamFrame>>* frames_to_send);
    int calcHeaderSize(bool short_header);


protected:
    int connection_source_ID;
    int connection_dest_ID;
    L3Address destination;
    QuicStreamArr *stream_arr;

};


} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICCONNECTION_H_ */
