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
#include "QuicReceiveQueue.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "QuicStreamArr.h"
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

#ifndef INET_APPLICATIONS_QUICAPP_QUICCONNECTIONSERVER_H_
#define INET_APPLICATIONS_QUICAPP_QUICCONNECTIONSERVER_H_

namespace inet {

class QuicStreamArr;
class QuicRecieveQueue;



class QuicConnectionServer : public QuicConnection {
public:
    QuicConnectionServer();
    QuicConnectionServer(L3Address destination);
    virtual ~QuicConnectionServer();

    void recievePacket(std::vector<stream_frame*> accepted_frames);
    int GetTotalConsumedBytes();
    int GetConnectionsRecieveOffset();
    int GetConnectionMaxWindow();
    bool GetIsOutOfOrder();
    std::list<Packet*>* getMaxStreamDataPackets();
    IntrusivePtr<inet::QuicACKFrame> getCurrentAckFrame();

    void performStateTransition(const QuicEventCode &event);
    Packet* ProcessEvent(const QuicEventCode &event,Packet* packet);

    Packet* ServerProcessHandshake(Packet* packet);
    void ProcessServerReceivedPacket(Packet* packet);
    void createAckFrame();

protected:
    int num_packets_recieved;

    std::list<int> receive_not_ACKED_queue;
    std::list<Packet*>* max_stream_data_packets_;


    //flow control server side parameters
    int inital_stream_window;


    // ACK control parameters
    int rcv_next;
    bool is_out_of_order;
    IntrusivePtr<inet::QuicACKFrame> current_Ack_frame;
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICCONNECTIONSERVER_H_ */
