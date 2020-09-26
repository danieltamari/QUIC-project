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
#include "StreamsData.h"
#include "ack_timer_msg_m.h"
#include "headers_and_frames/ACKFrame_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"
#include "QuicConnectionClient.h"
#include "QuicConnectionServer.h"
#include "headers_and_frames/connection_config_data_m.h"

#ifndef INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_
#define INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_

#define SENDER 1
#define RECEIVER 2

//enum Packet_type {HANDSHAKE =0,
//                  HANDSHAKE_RESPONSE,
//                  FIRST_STREAMS_DATA,
//                  ACK_PACKET,
//                 // MAX_STREAM_DATA,
//                 // MAX_DATA
//                    };

enum Packet_type {INITIAL = 0,
                  ZERO_RTT,
                  HANDSHAKE,
                  RETRY,
                  FIRST_STREAMS_DATA,
                  ACK_PACKET

                 // MAX_STREAM_DATA,
                 // MAX_DATA
                    };
namespace inet {

struct ack_range_info {
    std::vector<range*>* ack_range_arr;
    int arr_size;
    int ACK_range_count;
    int first_ack_range;
};



class ConnectionManager:  public cSimpleModule, public UdpSocket::ICallback{
public:
    ConnectionManager();
    virtual ~ConnectionManager();

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
    void sendPacket(Packet *packet,L3Address destAddr) ;
    void connectToUDPSocket();

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    QuicConnectionClient* AddNewConnection(int* connection_data, int connection_data_size,L3Address destination);
    bool isIDAvailable(int src_ID);
    ack_range_info* createAckRange(int arr[],int N,int smallest,int largest,int rcv_next);



protected:
    // state
     UdpSocket socket;
     L3Address destAddr;
     int localPort = -1, destPort = -1;
     std::vector<L3Address> destAddresses;
     int destAddrRNG = -1;
     std::list<QuicConnection*>* connections;
     std::map<int,ack_timer_msg*> ACK_timer_msg_map;
     int type = RECEIVER;
     bool connected;



};
Define_Module(ConnectionManager);


} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_ */
