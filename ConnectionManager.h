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
#include "QuicSendQueue.h"
#include "QuicReceiveQueue.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "QuicStreamArr.h"
#include "QuicData_m.h"
#include "QuicPacketHeader_m.h"
#include "StreamsData.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"
#include "QuicConnection.h"
#include "connection_config_data_m.h"

#ifndef INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_
#define INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_


enum Packet_type {HANDSHAKE =0,
                  HANDSHAKE_RESPONSE,
                  FIRST_STREAMS_DATA,
                    };

namespace inet {

class ConnectionManager:  public cSimpleModule, public UdpSocket::ICallback{
public:
    ConnectionManager();
    virtual ~ConnectionManager();

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
    virtual L3Address chooseDestAddr();
    void sendPacket(Packet *packet) ;
    void connectToUDPSocket();

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    int AddNewConnection(int* connection_data, int connection_data_size);
    bool isIDAvailable(int src_ID);

protected:
    // state
     UdpSocket socket;
     L3Address destAddr;
     int localPort = -1, destPort = -1;
     std::vector<L3Address> destAddresses;
     int destAddrRNG = -1;
     std::vector<QuicConnection>connections;

};
Define_Module(ConnectionManager);


} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_ */
