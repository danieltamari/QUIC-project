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

#include "ConnectionManager.h"
#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"

#include "inet/networklayer/common/EcnTag_m.h"
#include "inet/networklayer/common/IpProtocolId_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"


#define MAX_DELAY 0.0001
//#define MAX_DELAY 4.0
#define CHECK 12
#define DELAY_TIME 0.2

enum ConnectionEvent {
    RTO_EXPIRED_EVENT = 0,
    ACK_EXPIRED_EVENT,
    HANDSHAKE_TIMER_EVENT,
    RTO_INITIAL_EXPIRED_EVENT,
};


namespace inet {

ConnectionManager::ConnectionManager() {
    connections = new std::list<QuicConnection*>();
    connected=false;
}

ConnectionManager::~ConnectionManager() {
    // TODO Auto-generated destructor stub
}

void ConnectionManager::socketDataArrived(UdpSocket *socket, Packet *packet) {
    // process incoming packet
    int packet_type;
    int dest_ID_from_peer;
    int src_ID_from_peer;
    int packet_number;
    auto header = packet->peekAtFront<QuicPacketHeader>();

    b header_form = header->getHeader_form();
    if (header_form == b(1))  { // long header
        auto long_header = packet->peekAtFront<QuicLongHeader>();
        packet_type = long_header->getLong_packet_type();
        src_ID_from_peer = long_header->getSource_connection_ID();
        dest_ID_from_peer = long_header->getDest_connection_ID();




        switch (packet_type) {
            case INITIAL: {
                if (type == RECEIVER) { // HANDSHAKE PACKET (in server)
                    L3Address srcAddr = packet->getTag<L3AddressInd>()->getSrcAddress();
                    // create new connection at server's side
                    QuicConnectionServer *connection = new QuicConnectionServer(srcAddr);
                    // set connections IDs
                    if (isIDAvailable(dest_ID_from_peer)) {
                        connection->SetSourceID(dest_ID_from_peer);
                    }
                    else {
                        int new_dest_ID;
                        do {
                            new_dest_ID = std::rand();
                        }
                        while (!isIDAvailable(new_dest_ID));
                        connection->SetSourceID(new_dest_ID);
                    }
                    connection->SetDestID(src_ID_from_peer);
                    timer_msg* ACK_timer = new timer_msg();
                    ACK_timer_msg_map.insert({ dest_ID_from_peer, ACK_timer });
                    connections->push_back(connection);
                    Packet *handshake__response_packet = connection->ServerProcessHandshake(packet);
                    sendPacket(handshake__response_packet,connection->GetDestAddress());
                }


                else if (type == SENDER) { // HANDSHAKE RESPONSE PACKET (in client)
                    for (std::list<QuicConnection*>::iterator it =
                            connections->begin(); it != connections->end(); ++it) {
                        if ((*it)->GetSourceID() == dest_ID_from_peer) {
                            (*it)->SetDestID(src_ID_from_peer);
                            // cancel timeout on initial packet
                            Packet* initial_packet_copy = (dynamic_cast<QuicConnectionClient*>(*it))->findInitialPacket();
                            cancelEvent(initial_packet_copy);
                            (dynamic_cast<QuicConnectionClient*>(*it))->ProcessClientHandshakeResponse(packet);
                            // update transport parameters for future re-connection
                            old_transport_parameters* curr_transport_params = new old_transport_parameters;
                            curr_transport_params->connection_window_size = (dynamic_cast<QuicConnectionClient*>(*it))->getConnectionWindow();
                            curr_transport_params->max_payload = (dynamic_cast<QuicConnectionClient*>(*it))->getMaxPayload();
                            curr_transport_params->stream_window_size = (dynamic_cast<QuicConnectionClient*>(*it))->getStreamWindow();
                            curr_transport_params->destination = (*it)->GetDestAddress();
                            old_trans_parameters.push_back(curr_transport_params);
                            dynamic_cast<QuicConnectionClient*>(*it)->ProcessClientSend();
                            // go over all the available packets and send them
                            std::list<Packet*>* packets_to_send = (dynamic_cast<QuicConnectionClient*>(*it))->getPacketsToSend();
                            for (std::list<Packet*>::iterator it_packet = packets_to_send->begin();
                                    it_packet != packets_to_send->end(); ++it_packet) {
                                Packet* curr_packet_to_send=*it_packet;
                                // set RTO on copy packet
                                auto header = curr_packet_to_send->peekAtFront<QuicPacketHeader>();
                                int packet_number = header->getPacket_number();
                                Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacket(packet_number);
                                copy_packet->setKind(RTO_EXPIRED_EVENT);
                                simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->GetRto();
                                scheduleAt(simTime()+RTO,copy_packet);
                                sendPacket(curr_packet_to_send,(*it)->GetDestAddress());
                            }
                            packets_to_send->clear();
                            break;
                        }
                    }
                }

                break;
            }

            case ZERO_RTT: {
                for (std::list<QuicConnection*>::iterator it =
                        connections->begin(); it != connections->end(); ++it) {
                    int tmp = (*it)->GetSourceID();
                    if ((*it)->GetSourceID() == dest_ID_from_peer) {
                        dynamic_cast<QuicConnectionServer*>(*it)->ProcessServerReceivedPacket(packet);
                        // set ACK timer for the packet if needed
                        timer_msg* ACK_timer = this->ACK_timer_msg_map.at(dest_ID_from_peer);
                        if (!ACK_timer->isScheduled()){
                            simtime_t max_ack_delay = MAX_DELAY;
                            ACK_timer->setKind(ACK_EXPIRED_EVENT);
                            ACK_timer->setDest_connection_ID(dest_ID_from_peer); //set connection id for handle message use
                            dynamic_cast<QuicConnectionServer*>(*it)->createAckFrame();
                            scheduleAt(simTime()+max_ack_delay,ACK_timer);
                        }
                    }
                }
                processPacketFrames(packet, dest_ID_from_peer);
                break;
            }
        }
   }

    else { // short header -> 1-RTT packet
        auto short_header = packet->peekAtFront<QuicShortHeader>();
        packet_number = short_header->getPacket_number();
        dest_ID_from_peer = short_header->getDest_connection_ID();
        //packet_type = short_header->getPacket_type();


        if (type == RECEIVER) {
            for (std::list<QuicConnection*>::iterator it =
                    connections->begin(); it != connections->end(); ++it) {
                if ((*it)->GetSourceID() == dest_ID_from_peer) {
                    dynamic_cast<QuicConnectionServer*>(*it)->ProcessServerReceivedPacket(packet);
                    // set ACK timer for the packet if needed
                    timer_msg* ACK_timer = this->ACK_timer_msg_map.at(dest_ID_from_peer);
                    if (!ACK_timer->isScheduled()){
                        simtime_t max_ack_delay = MAX_DELAY;
                        ACK_timer->setKind(ACK_EXPIRED_EVENT);
                        ACK_timer->setDest_connection_ID(dest_ID_from_peer); //set connection id for handle message use
                        dynamic_cast<QuicConnectionServer*>(*it)->createAckFrame();
                        scheduleAt(simTime()+max_ack_delay,ACK_timer);
                    }
                 }

            }
        }
        processPacketFrames(packet, dest_ID_from_peer);
    }
}

void ConnectionManager::processPacketFrames(Packet* packet, int dest_ID_from_peer) {
    // break packet into frames
    b offset_in_packet = b(0);
    auto header = packet->popAtFront<QuicPacketHeader>();
    while (offset_in_packet < packet->getDataLength()) {
        auto curr_frame = packet->peekDataAt<QuicFrame>(offset_in_packet);
        int type = curr_frame->getFrame_type();
        switch (type) { // temporary - until receiving different types of frames
            case (STREAM_DATA): {
                for (std::list<QuicConnection*>::iterator it =
                         this->connections->begin(); it != connections->end(); ++it) {
                     if ((*it)->GetSourceID() == dest_ID_from_peer){
                      //   auto header = packet->popAtFront<QuicShortHeader>();
                         auto stream_frame = packet->peekDataAt<StreamFrame>(offset_in_packet);
                         dynamic_cast<QuicConnectionServer*>(*it)->ProcessStreamDataFrame(stream_frame);
                         break;
                     }
                }

            break;

            }
            case (ACK): {
                for (std::list<QuicConnection*>::iterator it =
                         this->connections->begin(); it != connections->end(); ++it) {
                     if ((*it)->GetSourceID() == dest_ID_from_peer){
                       //  auto header = packet->popAtFront<QuicShortHeader>();
                        // auto ACK_data = packet->peekData<ACKFrame>();
                         auto ACK_data = packet->peekDataAt<ACKFrame>(offset_in_packet);
                         // go over all the ACKED packets in this ACK frame
                         int first_ack_range = ACK_data->getFirst_ACK_range();
                         int ack_ranges_num = ACK_data->getACK_range_count();
                         int largest = ACK_data->getLargest_acknowledged();
                         int total_acked = first_ack_range+1;
                          for (int j = 0; j < ack_ranges_num; j++) {
                              range curr_ack_range = ACK_data->getACK_ranges(j);
                              total_acked += curr_ack_range.ACK_range_length;
                          }

                          packet_rcv_type* acked_packet_arr =new packet_rcv_type[total_acked];

                          acked_packet_arr[0].packet_number = largest;
                          if (ack_ranges_num==0){
                              acked_packet_arr[0].in_order=true;
                          }
                          else
                              acked_packet_arr[0].in_order=false;
                            int recover_index = 1;
                            int current_ack = largest;
                            int current_range = first_ack_range;
                            while (current_range != 0) {
                                current_ack--;
                                current_range--;
                                if (ack_ranges_num==0){
                                    acked_packet_arr[recover_index].in_order=true;
                                }
                                else
                                    acked_packet_arr[recover_index].in_order=false;
                                acked_packet_arr[recover_index].packet_number = current_ack;
                                recover_index++;
                            }

                            for (int i = 0; i < ack_ranges_num; i++) {
                                range curr_ack_range = ACK_data->getACK_ranges(i);
                                current_range = curr_ack_range.ACK_range_length;
                                current_ack -= curr_ack_range.gap;
                                while (current_range != 0) {
                                    current_ack--;
                                    current_range--;
                                    acked_packet_arr[recover_index].packet_number = current_ack;
                                    acked_packet_arr[recover_index].in_order=false;
                                    recover_index++;
                                }
                            }
                            for (int i = 0; i < total_acked; i++) {
                                EV<< "recover arr in place: "<< i << " is: " << acked_packet_arr[i].packet_number <<
                                        " is in order: " <<  acked_packet_arr[i].in_order << endl;
                            }

                         for (int i = 0; i < total_acked; i++) {
                             int current_packet = acked_packet_arr[i].packet_number;
                             // cancel timeout on acked packets
                             Packet* copy_received_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacket(current_packet);
                             if (copy_received_packet == NULL)
                                 continue;
                             cancelEvent(copy_received_packet); //cancel RTO timeout
                         }
                         (dynamic_cast<QuicConnectionClient*>(*it))->ProcessClientACK(acked_packet_arr,total_acked);
                         // cancel timeout on lost packets
                         std::list<Packet*>* lost_packets = (dynamic_cast<QuicConnectionClient*>(*it))->getLostPackets(largest);
                         for (std::list<Packet*>::iterator it_remove =
                                 lost_packets->begin(); it_remove != lost_packets->end(); ++it_remove) {
                             cancelEvent(*it_remove);
                         }
                         (dynamic_cast<QuicConnectionClient*>(*it))->ProcessClientSend();

                         std::list<Packet*>* packets_to_send = (dynamic_cast<QuicConnectionClient*>(*it))->getPacketsToSend();
                         for (std::list<Packet*>::iterator it_packet = packets_to_send->begin();
                                 it_packet != packets_to_send->end(); ++it_packet) {
                             Packet* curr_packet_to_send=*it_packet;
                             // setRTO on copy packet
                             auto header = curr_packet_to_send->peekAtFront<QuicPacketHeader>();
                             int packet_number = header->getPacket_number();
                             Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacket(packet_number);
                             simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->GetRto();
                             scheduleAt(simTime()+RTO,copy_packet);
                             sendPacket(curr_packet_to_send,(*it)->GetDestAddress());
                         }
                         packets_to_send->clear();
                         break;
                     }
                }
                break;
            }
        }
        offset_in_packet += curr_frame->getChunkLength();
    }
}

ack_range_info* ConnectionManager::createAckRange(int arr[],int N,int smallest,int largest,int rcv_next){

    // Initialize diff
        std::vector<int> missing;

//        for (int i = 0; i < N; i++) {
//            missing[i] = 0;
//        }

        int diff = arr[0] - 0;
        int j = 1;
        missing.push_back(arr[0] - 1) ;
        for (int i = 0; i < N; i++) {
            // Check if diff and arr[i]-i
            // both are equal or not
            if (arr[i] - i != diff) {
                // Loop for consecutiv
                // missing elements
                while (diff < arr[i] - i) {
                    missing.push_back(i + diff);
                        diff++;
                        j++;
                }
            }
        }
        int size = j-1;
        int* new_missing = new int[j];
        std::vector<range*>* ack_ranges_arr = new std::vector<range*>;
        int index = 0;
        while (j!=0){
            new_missing[index] = missing[j-1];
            j--;
            index++;
        }

        int first_ack_range = largest - new_missing[0] - 1;
        int current_gap = 1;
        //int current_ack_range = 0;
        for (int i = 0 ; i < size; i++) {
            if (new_missing[i+1] == new_missing[i] - 1) {
                current_gap++;
            }
            else {
                range* current_range =new range;
                current_range->ACK_range_length=new_missing[i] - new_missing[i + 1] - 1;
                current_range->gap=current_gap;
                //ack_ranges_arr[current_ack_range].ACK_range_length = new_missing[i] - new_missing[i + 1] - 1;
                //ack_ranges_arr[current_ack_range].gap = current_gap;

                //current_ack_range++;
                ack_ranges_arr->push_back(current_range);
                current_gap = 1;
            }
        }
        range* last_range=new range;
        //ack_ranges_arr[current_ack_range].ACK_range_length = 0;
        //ack_ranges_arr[current_ack_range].gap = new_missing[size] - rcv_next + 1;
        last_range->ACK_range_length=0;
        last_range->gap=new_missing[size] - rcv_next + 1;
        ack_ranges_arr->push_back(last_range);


        ack_range_info* ack_range_ = new ack_range_info;
        ack_range_->ack_range_arr=ack_ranges_arr;
        ack_range_->ACK_range_count=ack_ranges_arr->size();
        ack_range_->arr_size=ack_ranges_arr->size();
        ack_range_->first_ack_range=first_ack_range;
        return ack_range_;


}

void ConnectionManager::socketErrorArrived(UdpSocket *socket,
        Indication *indication) {
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void ConnectionManager::socketClosed(UdpSocket *socket) {
    //if (operationalState == State::STOPPING_OPERATION)
    //    startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void ConnectionManager::initialize() {
    localPort = par("localPort");
    destPort = par("destPort");
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
}

void ConnectionManager::handleMessage(cMessage *msg) {
    EV << "im hereeeeee in connection handle message in connection manager"
              << endl;
    if (msg->arrivedOn("socketIn"))
        socket.processMessage(msg);

    else if (msg->isSelfMessage()){
        if (msg->getKind() == RTO_EXPIRED_EVENT){
            EV << "RTO EXPIRED!!!!!!!!!!!!!!"<< endl;
            //RETRANSIMTIION AND CONGESION UPDATE
            Packet *rto_expired_packet = check_and_cast<Packet*>(msg);
            auto header = rto_expired_packet->peekAtFront<QuicPacketHeader>();
            int dest_id = header->getDest_connection_ID();
            for (std::list<QuicConnection*>::iterator it =
                     connections->begin(); it != connections->end(); ++it) {
                 if ((*it)->GetDestID() == dest_id){
                     dynamic_cast<QuicConnectionClient*>(*it)->processRexmitTimer(); //update congestion control after timeout
                     Packet* new_packet = (dynamic_cast<QuicConnectionClient*>(*it))->updatePacketNumber(rto_expired_packet);
                     dynamic_cast<QuicConnectionClient*>(*it)->createCopyPacket(new_packet);
                     // setRTO on copy packet
                     auto header_new = new_packet->peekAtFront<QuicPacketHeader>();
                     int packet_number = header_new->getPacket_number();
                     // if packet no longer in send_not_ACKed queue -> need to change
                     Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacket(packet_number);
                     copy_packet->setKind(RTO_EXPIRED_EVENT);
                     simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->GetRto();
                     scheduleAt(simTime()+RTO,copy_packet);
                     sendPacket(new_packet,(*it)->GetDestAddress());
                     EV<< "send retransmit of: "<< header->getPacket_number() << "new number is: "<< packet_number << endl;

                 }
            }
        }


        else if ((msg->getKind() == RTO_INITIAL_EXPIRED_EVENT)) {
            Packet *rto_expired_packet = check_and_cast<Packet*>(msg);
            auto header = rto_expired_packet->peekAtFront<QuicLongHeader>();
            int dest_id = header->getDest_connection_ID();
            for (std::list<QuicConnection*>::iterator it =
                     connections->begin(); it != connections->end(); ++it) {
                 if ((*it)->GetDestID() == dest_id){
                     Packet* copy_packet = rto_expired_packet->dup();
                     // setRTO on copy packet
                     copy_packet->setKind(RTO_INITIAL_EXPIRED_EVENT);
                     simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->GetRto();
                     scheduleAt(simTime()+RTO,rto_expired_packet);
                     sendPacket(copy_packet,(*it)->GetDestAddress());
                     EV<< "send retransmit of initial packet " << endl;
                 }
            }

        }

        else if (msg->getKind() == ACK_EXPIRED_EVENT) {
            timer_msg* ACK_timer_msg = dynamic_cast<timer_msg*>(msg);
            int dest_ID_from_peer = ACK_timer_msg->getDest_connection_ID();
            EV << "ACK timer EXPIRED!!!!!!!!!!!!!!"<< endl;
            for (std::list<QuicConnection*>::iterator it =
                    this->connections->begin(); it != connections->end(); ++it) {
                if ((*it)->GetSourceID() == dest_ID_from_peer) {
                    // create ACK packet
                    char msgName[32];
                    sprintf(msgName, "QUIC packet ACK");
                    Packet *ack = new Packet(msgName);
                    int dest_ID = (*it)->GetDestID();

                    // create short header
                    unsigned int dest_ID_len = (*it)->calcSizeInBytes(dest_ID);
                    auto QuicHeader_ = makeShared<QuicShortHeader>();
                    QuicHeader_->setHeader_form(b(0));
                    QuicHeader_->setDest_connection_ID(dest_ID);
                 //   QuicHeader_->setPacket_type(5);
                    int header_length = SHORT_HEADER_BASE_LENGTH + dest_ID_len;
                    QuicHeader_->setChunkLength(B(header_length));
                    ack->insertAtFront(QuicHeader_);


                    // create copy of ACK frame
                    IntrusivePtr<inet::ACKFrame> current_Ack_Frame = (dynamic_cast<QuicConnectionServer*>(*it))->getCurrentAckFrame();
                    auto ACK_frame_to_send = makeShared<ACKFrame>();
                    ACK_frame_to_send->setFrame_type(2);
                    std::list<int> not_acked_list=(dynamic_cast<QuicConnectionServer*>(*it))->GetNotAckedList();
                    int ack_range_count;
                    if (!not_acked_list.empty()){// we need to send out of order ACK
                        int rcv_next=(dynamic_cast<QuicConnectionServer*>(*it))->GetRcvNext();
                        int arr_index=0;
                        int list_size=not_acked_list.size();
                        int arr[list_size];
                         for (std::list<int>::iterator it_ack =
                                not_acked_list.begin(); it_ack != not_acked_list.end(); ++it_ack) {
                                arr[arr_index]=*it_ack;
                                arr_index++;
                        }
                        int largest=not_acked_list.back();
                        int smallest=not_acked_list.front();
                        ack_range_info* ack_range_info_=this->createAckRange(arr,list_size,smallest, largest,rcv_next);
                        ACK_frame_to_send->setACK_rangesArraySize(ack_range_info_->arr_size);
                        ack_range_count=ack_range_info_->arr_size;
                        for (int ii=0; ii<ack_range_count;ii++){
                            range* current_range = ack_range_info_->ack_range_arr->at(ii);
                            ACK_frame_to_send->setACK_ranges(ii, *current_range);
                        }
                        for (int ii=0; ii<ack_range_count;ii++){
                             range check = ACK_frame_to_send->getACK_ranges(ii);
                             EV << "range is: "<< check.ACK_range_length << "gap is " << check.gap<< endl;

                        }
                        ACK_frame_to_send->setACK_range_count(ack_range_count);
                        ACK_frame_to_send->setFirst_ACK_range(ack_range_info_->first_ack_range);
                        ACK_frame_to_send->setLargest_acknowledged(largest);
                        dynamic_cast<QuicConnectionServer*>(*it)->RcvInOrdedRst();
                    }
                    else {
    //                        if (dynamic_cast<QuicConnectionServer*>(*it)->GetListFlushed()){
    //                            ACK_frame_to_send->setACK_range_count(-1);
    //                            dynamic_cast<QuicConnectionServer*>(*it)->SetListFlushed(false);
    //                        }
                        //int first_ack_range = dynamic_cast<QuicConnectionServer*>(*it)->GetRcvInOrderAndRst()-1;
                        ACK_frame_to_send->setFirst_ACK_range(0); //no out of order ack,first_range_count=0;
                        int largest= dynamic_cast<QuicConnectionServer*>(*it)->getLargestInOrder();
                        ACK_frame_to_send->setLargest_acknowledged(largest);
                    }

                    ACK_frame_to_send->setChunkLength(B(sizeof(int)*(4+2*ack_range_count)));
                    ack->insertAtBack(ACK_frame_to_send);
                    // current_Ack_Frame;
                    sendPacket(ack,(*it)->GetDestAddress());
                }
            }

        }
        else if (msg->getKind() == HANDSHAKE_TIMER_EVENT){
            timer_msg* handshake_timer_msg = dynamic_cast<timer_msg*>(msg);
            int src_id = handshake_timer_msg->getDest_connection_ID();
            for (std::list<QuicConnection*>::iterator it =
                            connections->begin(); it != connections->end(); ++it) {
                if ((*it)->GetSourceID() == src_id) {
                    Packet *handshake_packet = (dynamic_cast<QuicConnectionClient*>(*it))->ProcessInitiateHandshake(src_id);
                    Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findInitialPacket();
                    copy_packet->setKind(RTO_INITIAL_EXPIRED_EVENT);
                    simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->GetRto();
                    scheduleAt(simTime()+RTO,copy_packet);

                    if ((dynamic_cast<QuicConnectionClient*>(*it))->GetReconnect()){
                        sendPacket(handshake_packet,(*it)->GetDestAddress());
                        for (std::vector<old_transport_parameters*>::iterator it_trans_params =
                                old_trans_parameters.begin(); it_trans_params != old_trans_parameters.end(); ++it_trans_params) {
                                if ((*it)->GetDestAddress() == (*it_trans_params)->destination) {
                                    int connection_window_size = (*it_trans_params)->connection_window_size;
                                    int stream_window_size = (*it_trans_params)->stream_window_size;
                                    int max_payload = (*it_trans_params)->max_payload;
                                    dynamic_cast<QuicConnectionClient*>(*it)->ProcessClientZeroRtt(connection_window_size, max_payload, stream_window_size);
                                    old_trans_parameters.erase(it_trans_params);
                                    break;
                                }
                        }

                        // go over all the available packets and send them
                        std::list<Packet*>* packets_to_send = (dynamic_cast<QuicConnectionClient*>(*it))->getPacketsToSend();
                        for (std::list<Packet*>::iterator it_packet = packets_to_send->begin();
                                it_packet != packets_to_send->end(); ++it_packet) {
                            Packet* curr_packet_to_send=*it_packet;
                            // set RTO on copy packet
                            auto header = curr_packet_to_send->peekAtFront<QuicPacketHeader>();
                            int packet_number = header->getPacket_number();
                            Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacket(packet_number);
                            copy_packet->setKind(RTO_EXPIRED_EVENT);
                            simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->GetRto();
                            scheduleAt(simTime()+RTO,copy_packet);
                            sendPacket(curr_packet_to_send,(*it)->GetDestAddress());
                        }
                        packets_to_send->clear();
                        //sendPacket(zero_rtt_packet,(*it)->GetDestAddress());
                   }
                    else
                        sendPacket(handshake_packet,(*it)->GetDestAddress());
                }
            }
        }
    }
    else {
        if (connected==false){
            connected=true;
            connectToUDPSocket();
        }
        if (msg->getKind() == SENDER) {
            type = SENDER;
            Packet *packet = check_and_cast<Packet*>(msg);
          //  uint32 total_bytes = (packet->getByteLength());
            auto packet_data = packet->peekData<connection_config_data>();
            int connection_data_size = packet_data->getConnection_dataArraySize();
            int* connection_data = new int [connection_data_size];
            for (int i=0; i<connection_data_size; i++) {
                connection_data[i] = packet_data->getConnection_data(i);
            }
            const char* connectAddress=packet_data->getConnectAddress();
            L3Address destination;
            L3AddressResolver().tryResolve(connectAddress, destination);
            bool reconnect = false;
            for (std::vector<old_transport_parameters*>::iterator it =
                    old_trans_parameters.begin(); it != old_trans_parameters.end(); ++it) {
                    if (destination == (*it)->destination) {
                        reconnect=true;
                        break;
                    }
            }


            QuicConnectionClient* connection = AddNewConnection(connection_data, connection_data_size,destination,reconnect);
            int src_ID_;
            do {
                src_ID_ = std::rand();
            }
            while(!isIDAvailable(src_ID_));
            connection->SetSourceID(src_ID_);


            timer_msg* handshake_timer = new timer_msg ("handshake timer");
            handshake_timer->setKind(HANDSHAKE_TIMER_EVENT);
            handshake_timer->setDest_connection_ID(src_ID_);
            if (reconnect){
                scheduleAt(simTime(),handshake_timer);
            }
            else
            {
               // destAddresses_vector.push_back(destination);
                scheduleAt(simTime()+DELAY_TIME,handshake_timer);
            }
        }
        else if (msg->getKind()== ADDSTREAMS){
            Packet *packet = check_and_cast<Packet*>(msg);
            auto packet_data = packet->peekData<connection_config_data>();
            int connection_data_size = packet_data->getConnection_dataArraySize();
            int* connection_data = new int [connection_data_size];
            for (int i=0; i<connection_data_size; i++) {
                connection_data[i] = packet_data->getConnection_data(i);
            }
            const char* connectAddress=packet_data->getConnectAddress();
            L3Address destination;
            L3AddressResolver().tryResolve(connectAddress, destination);
            for (std::list<QuicConnection*>::iterator it =
                            connections->begin(); it != connections->end(); ++it) {
                    if ((*it)->GetDestAddress()==destination) {
                        int current_stream_number=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamNumber();
                        int initial_flow_contro_window =  (dynamic_cast<QuicConnectionClient*>(*it))->getStreamWindow();
                        for (int i=0; i<connection_data_size; i++) {
                            (dynamic_cast<QuicConnectionClient*>(*it))->AddNewStream(connection_data[i], i+current_stream_number);
                            (dynamic_cast<QuicConnectionClient*>(*it))->updateStreamFlowControl(i+current_stream_number, initial_flow_contro_window);
                        }
                        dynamic_cast<QuicConnectionClient*>(*it)->setStreamNumber(connection_data_size+current_stream_number);
                        break;
                    }
            }
        }
    }
}






QuicConnectionClient* ConnectionManager::AddNewConnection(int* connection_data, int connection_data_size,L3Address destination, bool reconnect) {
    QuicConnectionClient* connection = new QuicConnectionClient(connection_data, connection_data_size,destination,reconnect);
    connections->push_back(connection);
    return connection;
}



void ConnectionManager::connectToUDPSocket() {

    socket.bind(localPort);
}

void ConnectionManager::sendPacket(Packet *packet,L3Address destAddr) {

    //L3Address destAddr = chooseDestAddr(module_number);
    socket.sendTo(packet, destAddr, this->destPort);
}

bool ConnectionManager::isIDAvailable(int dest_ID) {
    for (std::list<QuicConnection*>::iterator it = this->connections->begin();
            it != connections->end(); ++it) {
        if ((*it)->GetSourceID() == dest_ID)
            return false;
    }
    return true;
}

} /* namespace inet */
