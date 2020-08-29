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

#ifndef INET_APPLICATIONS_QUICAPP_QUICCONNECTIONSENDQUEUE_H_
#define INET_APPLICATIONS_QUICAPP_QUICCONNECTIONSENDQUEUE_H_

#include "inet/common/packet/Packet.h"

namespace inet {

class QuicConnectionSendQueue {
public:
    QuicConnectionSendQueue();
    virtual ~QuicConnectionSendQueue();
    void addPacket(Packet* packet);


protected:
    std::vector<Packet*> sent_without_ACK_packets;

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICCONNECTIONSENDQUEUE_H_ */
