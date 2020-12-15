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
    sent_not_Acked_packets = new std::list<Packet*>();
    lost_packets = new std::list<Packet*>();

}


QuicSendQueue::~QuicSendQueue() {
    // TODO Auto-generated destructor stub
}


void QuicSendQueue::createCopyPacket(Packet* original_packet) {
    // create copy
    Packet* copy_packet_to_send = original_packet->dup();
    copy_packet_to_send->setTimestamp(simTime());
    sent_not_Acked_packets->push_back(copy_packet_to_send);
}


Packet* QuicSendQueue::findPacketInSendQueue(int packet_number) {
    Packet* packet_to_peek = NULL;
    for (std::list<Packet*>::iterator it =
            sent_not_Acked_packets->begin(); it != sent_not_Acked_packets->end(); ++it) {
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
            sent_not_Acked_packets->begin(); it != sent_not_Acked_packets->end(); ++it) {
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
    std::list<Packet*>::iterator it = sent_not_Acked_packets->begin();
    while (it != sent_not_Acked_packets->end()) {
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
    sent_not_Acked_packets->remove(packet_to_remove);
}


Packet* QuicSendQueue::removeFromSendQueueByNumber(int packet_number) {
    Packet* packet_to_remove = NULL;
    for (std::list<Packet*>::iterator it =
            sent_not_Acked_packets->begin(); it != sent_not_Acked_packets->end(); ++it) {
            auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
            if (current_header->getPacket_number()==packet_number){
                packet_to_remove=*it;
                sent_not_Acked_packets->erase(it);
                break;
            }
    }
    return packet_to_remove;
}


std::list<Packet*>* QuicSendQueue::getLostPackets() {
    return lost_packets;
}


std::vector<int>* QuicSendQueue::updateLostPackets(int largest) {
    lost_packets->clear();
    int packet_threshold = largest - kPacketThreshold; // all packets under packet_threshold are lost
    std::vector<int>* lost_packets_numbers = new std::vector<int>();
    for (std::list<Packet*>::iterator it =
            sent_not_Acked_packets->begin(); it != sent_not_Acked_packets->end(); ++it) {
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


std::list<Packet*>* QuicSendQueue::getLostAcksPackets(int get_before_number) {
    std::list<Packet*>* lost_ACK_packets = new std::list<Packet*>();
    std::list<Packet*>::iterator it = sent_not_Acked_packets->begin();
    while (it != sent_not_Acked_packets->end()) {
        auto current_header=(*it)->peekAtFront<QuicPacketHeader>();
        Packet* packet_to_remove;
        if (current_header->getPacket_number() <= get_before_number){
            packet_to_remove=*it;
            it++;
            sent_not_Acked_packets->remove(packet_to_remove);
            lost_ACK_packets->push_back(packet_to_remove);
        }
        else {
            it++;
        }
    }
    return lost_ACK_packets;
}


// TEMP
void QuicSendQueue::printSendNotAcked(){
    // print sent_not_Acked_packets:
    EV << "########### sent_not_Acked_packets: ###############" << endl;
    for (std::list<Packet*>::iterator it =
            sent_not_Acked_packets->begin(); it != sent_not_Acked_packets->end(); ++it) {
        auto temp_header =(*it)->peekAtFront<QuicPacketHeader>();
        EV << "packet number: " << temp_header->getPacket_number() << endl;
    }
}


} /* namespace inet */
