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

#include "inet/common/packet/Packet.h"
#include "headers_and_frames/QuicPacketHeader_m.h"
#include "headers_and_frames/QuicLongHeader_m.h"

#ifndef INET_APPLICATIONS_QUICAPP_QUICSENDQUEUE_H_
#define INET_APPLICATIONS_QUICAPP_QUICSENDQUEUE_H_

#define kPacketThreshold 3

namespace inet {


class QuicSendQueue {
public:
    QuicSendQueue();
    virtual ~QuicSendQueue();
    void createCopyPacket(Packet* original_packet);
    void removeFromSendQueue(Packet* packet_to_remove);
    Packet* findPacketInSendQueue(int packet_number);
    Packet* findInitialPacket();
    std::list<Packet*>* getPacketsToCancel(int cancel_RTO_before_number);
    Packet* removeFromSendQueueByNumber(int packet_number);
    std::list<Packet*>* getLostPackets();
    std::vector<int>* updateLostPackets(int largest);
    // TEMP
    void printSendNotAcked();


protected:
    std::list<Packet*>* send_not_ACKED_queue;
    std::list<Packet*>* lost_packets;

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICSENDQUEUE_H_ */
