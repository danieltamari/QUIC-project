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

#include "QuicSendQueue.h"

namespace inet {

QuicSendQueue::QuicSendQueue() {
    send_not_ACKED_queue = new std::list<Packet*>();
    lost_packets = new std::list<Packet*>();

}



QuicSendQueue::~QuicSendQueue() {
    // TODO Auto-generated destructor stub
}


void QuicSendQueue::createCopyPacket(Packet* original_packet) {
    // create copy
    Packet* copy_packet_to_send = original_packet->dup();
    copy_packet_to_send->setTimestamp(simTime());
    send_not_ACKED_queue->push_back(copy_packet_to_send);
}





Packet* QuicSendQueue::findPacketInSendQueue(int packet_number) {
    Packet* packet_to_peek = NULL;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number()==packet_number){
                packet_to_peek=*it;
                break;
            }
    }
    return packet_to_peek;
}


Packet* QuicSendQueue::findInitialPacket() {
    Packet* packet_to_peek = NULL;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getHeader_form() == b(1)){
                auto long_header=(*it)->peekAtFront<QuicLongHeader>();
                if (long_header->getLong_packet_type() == 0){
                    packet_to_peek=*it;
                    break;
                }
            }
    }
    return packet_to_peek;
}


std::list<Packet*>* QuicSendQueue::getPacketsToCancel(int cancel_RTO_before_number) {
    std::list<Packet*>* cancel_timer_packets = new std::list<Packet*>();
    std::list<Packet*>::iterator it = send_not_ACKED_queue->begin();
    while (it != send_not_ACKED_queue->end()) {
        auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
        Packet* packet_to_cancel;
        if (current_header->getPacket_number() <= cancel_RTO_before_number){
            packet_to_cancel=*it;
            it++;
            cancel_timer_packets->push_back(packet_to_cancel);
        }
        else {
            it++;
        }
    }
    return cancel_timer_packets;
}


void QuicSendQueue::removeFromSendQueue(Packet* packet_to_remove) {
    send_not_ACKED_queue->remove(packet_to_remove);
}


Packet* QuicSendQueue::removeFromSendQueueByNumber(int packet_number) {
    Packet* packet_to_remove = NULL;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number()==packet_number){
                packet_to_remove=*it;
                send_not_ACKED_queue->erase(it);
                break;
            }
    }
    return packet_to_remove;
}


std::list<Packet*>* QuicSendQueue::getAckedPackets(int first_acked) {
    std::list<Packet*>* acked_packets = new std::list<Packet*>();
    std::list<Packet*>::iterator it = send_not_ACKED_queue->begin();
    while (it != send_not_ACKED_queue->end()) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number() < first_acked){
                Packet* copy_of_ACKED_packet = (*it)->dup();
                send_not_ACKED_queue->erase(it);
                it++;
               // send_not_ACKED_queue->remove(copy_of_ACKED_packet);
                acked_packets->push_back(copy_of_ACKED_packet);
             //   Packet* p_temp = copy_of_ACKED_packet->dup();
            }
            else
                it++;
    }
    return acked_packets;
}


std::list<Packet*>* QuicSendQueue::getLostPackets() {
    return lost_packets;
}


std::vector<int>* QuicSendQueue::updateLostPackets(int largest) {
    lost_packets->clear();
    int packet_threshold = largest - kPacketThreshold; // all packets under packet_threshold are lost
    std::vector<int>* lost_packets_numbers = new std::vector<int>();
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
        auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
        Packet* packet_to_remove;
        if (current_header->getPacket_number() <= packet_threshold){
            lost_packets_numbers->push_back(current_header->getPacket_number());
            packet_to_remove=*it;
            lost_packets->push_back(packet_to_remove);
        }
    }
    return lost_packets_numbers;
}

// TEMP
void QuicSendQueue::printSendNotAcked(){
    // print send_not_ACKED_queue:
    EV << "########### send_not_ACKED_queue: ###############" << endl;
    for (std::list<Packet*>::iterator it =
            send_not_ACKED_queue->begin(); it != send_not_ACKED_queue->end(); ++it) {
        auto temp_header =(*it)->peekAtFront<QuicPacketHeader>();
        EV << "packet number: " << temp_header->getPacket_number() << endl;
    }
}

} /* namespace inet */
