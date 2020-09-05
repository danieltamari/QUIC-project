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
#include "QuicConnection.h"
#include "QuicSendQueue.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "QuicStreamArr.h"
#include "QuicNewReno.h"
#include "QuicData_m.h"
#include "QuicPacketHeader_m.h"
#include "QuicHandShakeData_m.h"
#include "MaxStreamData_m.h"
#include "StreamsData.h"
#include "QuicACKFrame_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"


#ifndef INET_APPLICATIONS_QUICAPP_QuicConnection_H_
#define INET_APPLICATIONS_QUICAPP_QuicConnection_H_

namespace inet {

class QuicStreamArr;
class QuicSendQueue;
class QuicNewRENO;



class QuicConnectionClient : public QuicConnection { //public cSimpleModule, public UdpSocket::ICallback{
public:

    QuicConnectionClient();
    QuicConnectionClient(int* connection_data, int connection_data_size,L3Address destination);
    virtual ~QuicConnectionClient();


    Packet* createQuicDataPacket(StreamsData* streams_data);
    StreamsData* CreateSendData(int max_payload, int connection_window);
    void AddNewStream(int stream_size,int index);
    Packet* findPacket(int packet_number);

    void performStateTransition(const QuicEventCode &event);
    Packet* ProcessEvent(const QuicEventCode &event,Packet* packet);

    Packet* ProcessInitiateHandshake(Packet* packet);
    Packet* ProcessClientHandshakeResponse(Packet* packet);
    Packet* ProcessClientSend(Packet* packet);

    Packet* RemovePacketFromQueue(int packet_number);
    void updateFlowControl(Packet* acked_packet);
    void updateMaxStreamData(int stream_id, int max_stream_data_offset);


    void UpdateRtt(simtime_t acked_time);

    simtime_t GetRto();
    void processRexmitTimer();


protected:
    int packet_counter; // counter to assign each packet header a unique packet number
    int num_packets_sent;
    int send_una; // sent and unacknowledged bytes so far
    std::list<Packet*> send_not_ACKED_queue;


    //flow control client side parameters
    int max_payload;

    // ACK control parameters
    int last_rcvd_ACK;
    int dup_ACKS;
    QuicNewReno* congestion_alg;

};


//Define_Module(QuicConnection);

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QuicConnection_H_ */
