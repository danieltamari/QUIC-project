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
#include "headers_and_frames/QuicData_m.h"
#include "headers_and_frames/QuicPacketHeader_m.h"
#include "headers_and_frames/QuicLongHeader_m.h"
#include "headers_and_frames/QuicShortHeader_m.h"
#include "headers_and_frames/QuicHandShakeData_m.h"
#include "headers_and_frames/ACKFrame_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/ModuleAccess.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include <omnetpp.h>


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
    Packet* ServerProcessHandshake(Packet* packet);
    bool ProcessServerReceivedPacket(Packet* packet);
    void ProcessStreamDataFrame(inet::Ptr<const StreamFrame> stream_frame);
    int GetRcvNext();
    bool GetIsOutOfOrder();
    int GetCurrLargest();
    std::list<int> GetNotAckedList();
    int getLargestInOrder();
    int GetRcvInOrderAndRst();
    void SetCurrLargest(int largest);
    void setLargestWithRcvNext();


protected:
    int num_packets_recieved;
    QuicReceiveQueue* receive_queue;
    std::list<int> receive_not_ACKED_queue;

    //flow control server side parameters
    int inital_stream_window;

    // ACK control parameters
    int current_largest;
    int rcv_next;
    //int rcv_in_order;
    bool is_out_of_order;
    IntrusivePtr<inet::ACKFrame> current_Ack_frame;


};


} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICCONNECTIONSERVER_H_ */
