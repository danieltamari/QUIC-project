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


#define MAX_DELAY 0.00001
#define DELAY_TIME 0
#define THROUGHPUT_GAP 0.1
#define LONG_THROUGHPUT_GAP 1


enum ConnectionEvent {
    RTO_EXPIRED_EVENT = 0,
    ACK_EXPIRED_EVENT,
    HANDSHAKE_TIMER_EVENT,
    RTO_INITIAL_EXPIRED_EVENT,
    UPDATE_THROUGHPUT,
    UPDATE_THROUGHPUT_LONG
};


namespace inet {

ConnectionManager::ConnectionManager() {
    connections = new std::list<QuicConnection*>();
    connected=false;
    throughput_timer = new timer_msg("throughput timer");
    throughput_timer_long =new timer_msg("throughput long timer");
    counter = 0;
    latency_index=1;

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
                        connection->setSourceID(dest_ID_from_peer);
                        timer_msg* ACK_timer = new timer_msg();
                        ACK_timer_msg_map.insert({ dest_ID_from_peer, ACK_timer });
                        // #### REMOVE THIS
                        latency_connection_map.insert({ dest_ID_from_peer, latency_index });
                        latency_index++;
                    }
                    else {
                        int new_dest_ID;
                        do {
                            new_dest_ID = std::rand();
                        }
                        while (!isIDAvailable(new_dest_ID));
                        connection->setSourceID(new_dest_ID);
                        timer_msg* ACK_timer = new timer_msg();
                        ACK_timer_msg_map.insert({ new_dest_ID, ACK_timer });
                        // #### REMOVE THIS
                        int old_latency=latency_connection_map.at(dest_ID_from_peer);
                        //latency_connection_map.erase(dest_ID_from_peer);
                        latency_connection_map.insert({ new_dest_ID, old_latency });
                    }
                    connection->setDestID(src_ID_from_peer);
                    connections->push_back(connection);
                    Packet *handshake__response_packet = connection->ServerProcessHandshake(packet);
                    sendPacket(handshake__response_packet,connection->getDestAddress());
                }


                else if (type == SENDER) { // HANDSHAKE RESPONSE PACKET (in client)
                    for (std::list<QuicConnection*>::iterator it =
                            connections->begin(); it != connections->end(); ++it) {
                        if ((*it)->getSourceID() == dest_ID_from_peer) {
                            (*it)->setDestID(src_ID_from_peer);
                            // cancel timeout on initial packet
                            Packet* initial_packet_copy = (dynamic_cast<QuicConnectionClient*>(*it))->findInitialPacket();
                            cancelEvent(initial_packet_copy);
                            (dynamic_cast<QuicConnectionClient*>(*it))->ProcessClientHandshakeResponse(packet);
                            // update transport parameters for future re-connection
                            old_transport_parameters* curr_transport_params = new old_transport_parameters;
                            curr_transport_params->connection_window_size = (dynamic_cast<QuicConnectionClient*>(*it))->getConnectionWindow();
                            curr_transport_params->max_payload = (dynamic_cast<QuicConnectionClient*>(*it))->getMaxPayload();
                            curr_transport_params->stream_window_size = (dynamic_cast<QuicConnectionClient*>(*it))->getStreamWindow();
                            curr_transport_params->destination = (*it)->getDestAddress();
                            old_trans_parameters.push_back(curr_transport_params);
                            // record window change and RTT measurment
                            int send_window = (dynamic_cast<QuicConnectionClient*>(*it))->getSendWindow();
                            emit(snd_wnd_Signal, send_window);
                            ////// update signal
                            emit(bytes_sent_signal, 0);
                            emit(bytes_sent_with_ret_signal, 0);

                            dynamic_cast<QuicConnectionClient*>(*it)->ProcessClientSend();
                            // go over all the available packets and send them
                            std::list<Packet*>* packets_to_send = (dynamic_cast<QuicConnectionClient*>(*it))->getPacketsToSend();
                            for (std::list<Packet*>::iterator it_packet = packets_to_send->begin();
                                    it_packet != packets_to_send->end(); ++it_packet) {
                                Packet* curr_packet_to_send=*it_packet;
                                // set RTO on copy packet
                                auto header = curr_packet_to_send->peekAtFront<QuicPacketHeader>();
                                int packet_number = header->getPacket_number();
                                Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacketInSendQueue(packet_number);
                                copy_packet->setKind(RTO_EXPIRED_EVENT);
                                simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->getRto();
                                scheduleAt(simTime()+RTO,copy_packet);
                                sendPacket(curr_packet_to_send,(*it)->getDestAddress());
                            }
                            packets_to_send->clear();

                            // record bytes_sent
                            int bytes_sent = (dynamic_cast<QuicConnectionClient*>(*it))->getNumBytesSent();
                            int bytes_sent_with_ret = (dynamic_cast<QuicConnectionClient*>(*it))->getNumBytesSentWithRet();
                            emit(bytes_sent_signal, bytes_sent);
                            emit(bytes_sent_with_ret_signal,bytes_sent_with_ret);

                            int total_bytes_in_curr_send = (dynamic_cast<QuicConnectionClient*>(*it))->getTotalBytesInCurrSend();
                            int new_bytes_in_curr_send = (dynamic_cast<QuicConnectionClient*>(*it))->getNewBytesInCurrSend();
                            emit(total_bytes_in_curr_send_signal, total_bytes_in_curr_send);
                            emit(new_bytes_in_curr_send_signal, new_bytes_in_curr_send);

                            int stream0_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(0);
                            int stream1_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(1);
                            int stream2_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(2);
                            int stream3_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(3);
                            int stream4_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(4);
                            int stream5_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(5);
                            int stream6_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(6);
                            int stream7_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(7);
                            int stream8_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(8);
                            int stream9_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(9);
                            emit (stream0_send_bytes_signal,stream0_offset);
                            emit (stream1_send_bytes_signal,stream1_offset);
                            emit (stream2_send_bytes_signal,stream2_offset);
                            emit (stream3_send_bytes_signal,stream3_offset);
                            emit (stream4_send_bytes_signal,stream4_offset);
                            emit (stream5_send_bytes_signal,stream5_offset);
                            emit (stream6_send_bytes_signal,stream6_offset);
                            emit (stream7_send_bytes_signal,stream7_offset);
                            emit (stream8_send_bytes_signal,stream8_offset);
                            emit (stream9_send_bytes_signal,stream9_offset);
                            break;
                        }
                    }
                }

                break;
            }

            case ZERO_RTT: {
                bool new_data = false;
                for (std::list<QuicConnection*>::iterator it =
                        connections->begin(); it != connections->end(); ++it) {

                    if ((*it)->getSourceID() == dest_ID_from_peer) {
                        simtime_t t_sent = packet->getTimestamp();
                        simtime_t now = simTime();
                        simtime_t latency = now - t_sent;
                        EV << "latency is: " << latency*1000 << " ms" << endl;
                        int latency_index= latency_connection_map.at(dest_ID_from_peer);
                        if (latency_index==1){
                            emit(latency_signal,latency*1000);
                        }
                        else if (latency_index==2){
                            emit(latency_signal_second,latency*1000);
                        }
                        else if (latency_index==3){
                            emit(latency_signal_third,latency*1000);
                        }
                        else
                            emit(latency_signal_fourth,latency*1000);

                        new_data = dynamic_cast<QuicConnectionServer*>(*it)->ProcessServerReceivedPacket(packet);
                        // set ACK timer for the packet if needed
                        timer_msg* ACK_timer = ACK_timer_msg_map.at(dest_ID_from_peer);
                        if (!ACK_timer->isScheduled()){
                            simtime_t max_ack_delay = MAX_DELAY;
                            ACK_timer->setKind(ACK_EXPIRED_EVENT);
                            ACK_timer->setDest_connection_ID(dest_ID_from_peer); //set connection id for handle message use
                            scheduleAt(simTime()+max_ack_delay,ACK_timer);
                        }
                    }
                }

                processPacketFrames(packet, dest_ID_from_peer, new_data);
                break;
            }
        }
   }

    else { // short header -> 1-RTT packet
        auto short_header = packet->peekAtFront<QuicShortHeader>();
        packet_number = short_header->getPacket_number();
        dest_ID_from_peer = short_header->getDest_connection_ID();
        bool new_data = false;

        if (type == RECEIVER) {
            for (std::list<QuicConnection*>::iterator it =
                    connections->begin(); it != connections->end(); ++it) {
                simtime_t t_sent = packet->getTimestamp();
                simtime_t now = simTime();
                simtime_t latency = now - t_sent ;
                EV << "latency is: " << latency*1000 << " ms" << endl;
                int latency_index = latency_connection_map.at(dest_ID_from_peer);
                if (latency_index==1){
                    emit(latency_signal,latency*1000);
                }
                else if (latency_index==2){
                    emit(latency_signal_second,latency*1000);
                }
                else if (latency_index==3){
                    emit(latency_signal_third,latency*1000);
                }
                else
                    emit(latency_signal_fourth,latency*1000);

                if ((*it)->getSourceID() == dest_ID_from_peer) {
                    new_data = dynamic_cast<QuicConnectionServer*>(*it)->ProcessServerReceivedPacket(packet);
                    // set ACK timer for the packet if needed
                    timer_msg* ACK_timer = ACK_timer_msg_map.at(dest_ID_from_peer);
                    if (!ACK_timer->isScheduled()){
                        simtime_t max_ack_delay = MAX_DELAY;
                        ACK_timer->setKind(ACK_EXPIRED_EVENT);
                        ACK_timer->setDest_connection_ID(dest_ID_from_peer); //set connection id for handle message use
                        scheduleAt(simTime()+max_ack_delay,ACK_timer);
                    }
                 }
            }
        }
        processPacketFrames(packet, dest_ID_from_peer, new_data);
    }
}


void ConnectionManager::processPacketFrames(Packet* packet, int dest_ID_from_peer, bool new_data) {
    // break packet into frames
    b offset_in_packet = b(0);
    auto header = packet->popAtFront<QuicPacketHeader>();

    while (offset_in_packet < packet->getDataLength()) {
        auto curr_frame = packet->peekDataAt<QuicFrame>(offset_in_packet);
        int type = curr_frame->getFrame_type();
        switch (type) { // temporary - until receiving different types of frames
            case (STREAM_DATA): {
                for (std::list<QuicConnection*>::iterator it =
                         connections->begin(); it != connections->end(); ++it) {
                     if ((*it)->getSourceID() == dest_ID_from_peer){
                         auto stream_frame = packet->peekDataAt<StreamFrame>(offset_in_packet);
                         if (new_data) {
                             EV << "##############" << endl;
                             EV << "packet number is: " << header->getPacket_number() << endl;
                             dynamic_cast<QuicConnectionServer*>(*it)->ProcessStreamDataFrame(stream_frame);
                             EV << "##############" << endl;
                         }
                         break;
                     }
                }

                break;
            }

            case (ACK): {
                for (std::list<QuicConnection*>::iterator it =
                         connections->begin(); it != connections->end(); ++it) {
                     if ((*it)->getSourceID() == dest_ID_from_peer) {
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
                             if (ack_ranges_num==0)
                                 acked_packet_arr[recover_index].in_order=true;

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
                             Packet* copy_received_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacketInSendQueue(current_packet);
                             if (copy_received_packet == NULL)
                                 continue;
                             EV << " canceling RTO on packet " << current_packet << endl;
                             cancelEvent(copy_received_packet); //cancel RTO timeout
                         }

                         /////################ cancel timeout on lost ACK packet
                         if (acked_packet_arr[total_acked-1].in_order==true){
                             int cancel_RTO_before_number=acked_packet_arr[total_acked-1].packet_number-1;
                             std::list<Packet*>* packets_to_cancel = (dynamic_cast<QuicConnectionClient*>(*it))->getPacketsToCancel(cancel_RTO_before_number);
                             for (std::list<Packet*>::iterator it_packet = packets_to_cancel->begin();
                                     it_packet != packets_to_cancel->end(); ++it_packet) {
                                 cancelEvent(*it_packet);
                               //  Packet* packet_copy = (*it_packet)->dup();
                               //  (dynamic_cast<QuicConnectionClient*>(*it))->updateStreamInfo(*it_packet);
                              //   (dynamic_cast<QuicConnectionClient*>(*it))->updateFlowControl(packet_copy);
                                 auto headerrr = (*it_packet)->peekAtFront<QuicPacketHeader>();
                                 EV << " canceling RTO on packet " << headerrr->getPacket_number() << endl;
                             }
                             delete(packets_to_cancel);
                         }

                         (dynamic_cast<QuicConnectionClient*>(*it))->ProcessClientACK(acked_packet_arr,total_acked);
                         (dynamic_cast<QuicConnectionClient*>(*it))->updateLostPackets(largest);
                         // cancel timeout on lost packets
                         EV << " ***********canceling RTO on lost packets****** "<< endl;
                         std::list<Packet*>* lost_packets = (dynamic_cast<QuicConnectionClient*>(*it))->getLostPackets();
                         for (std::list<Packet*>::iterator it_remove =
                                 lost_packets->begin(); it_remove != lost_packets->end(); ++it_remove) {
                             cancelEvent(*it_remove);


                         }

                         // record window change and RTT measurment
                         int send_window = (dynamic_cast<QuicConnectionClient*>(*it))->getSendWindow();
                         emit(snd_wnd_Signal, send_window);
                         simtime_t RTT = (dynamic_cast<QuicConnectionClient*>(*it))->getRtt();
                         emit(rtt_signal, RTT);

                         (dynamic_cast<QuicConnectionClient*>(*it))->ProcessClientSend();
                         std::list<Packet*>* packets_to_send = (dynamic_cast<QuicConnectionClient*>(*it))->getPacketsToSend();
                         if (packets_to_send->empty()) {
                             // if all streams ended -> close connection
                             if((dynamic_cast<QuicConnectionClient*>(*it))->getEndConnection()) {
                                 EV << "connection source id " << (*it)->getSourceID() << " and dest id " << (*it)->getDestID() << " ended at client" << endl;
                                 cancelEvent(throughput_timer);
                                 cancelEvent(throughput_timer_long);
                                 connections->erase(it);
                                 delete (*it);
                                 break;
                             }
                         }


                         for (std::list<Packet*>::iterator it_packet = packets_to_send->begin();
                                 it_packet != packets_to_send->end(); ++it_packet) {
                             Packet* curr_packet_to_send=*it_packet;
                             // setRTO on copy packet
                             auto header = curr_packet_to_send->peekAtFront<QuicPacketHeader>();
                             int packet_number = header->getPacket_number();
                             Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacketInSendQueue(packet_number);
                             simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->getRto();
                             scheduleAt(simTime()+RTO,copy_packet);
                             sendPacket(curr_packet_to_send,(*it)->getDestAddress());
                         }
                         packets_to_send->clear();

                         // record bytes_sent
                         int bytes_sent = (dynamic_cast<QuicConnectionClient*>(*it))->getNumBytesSent();
                         int bytes_sent_with_ret = (dynamic_cast<QuicConnectionClient*>(*it))->getNumBytesSentWithRet();
                         emit(bytes_sent_signal, bytes_sent);
                         emit(bytes_sent_with_ret_signal,bytes_sent_with_ret);

                         int total_bytes_in_curr_send = (dynamic_cast<QuicConnectionClient*>(*it))->getTotalBytesInCurrSend();
                         int new_bytes_in_curr_send = (dynamic_cast<QuicConnectionClient*>(*it))->getNewBytesInCurrSend();
                         emit(total_bytes_in_curr_send_signal, total_bytes_in_curr_send);
                         emit(new_bytes_in_curr_send_signal, new_bytes_in_curr_send);

                         int stream0_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(0);
                         int stream1_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(1);
                         int stream2_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(2);
                         int stream3_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(3);
                         int stream4_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(4);
                         int stream5_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(5);
                         int stream6_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(6);
                         int stream7_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(7);
                         int stream8_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(8);
                         int stream9_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(9);
                         emit (stream0_send_bytes_signal,stream0_offset);
                         emit (stream1_send_bytes_signal,stream1_offset);
                         emit (stream2_send_bytes_signal,stream2_offset);
                         emit (stream3_send_bytes_signal,stream3_offset);
                         emit (stream4_send_bytes_signal,stream4_offset);
                         emit (stream5_send_bytes_signal,stream5_offset);
                         emit (stream6_send_bytes_signal,stream6_offset);
                         emit (stream7_send_bytes_signal,stream7_offset);
                         emit (stream8_send_bytes_signal,stream8_offset);
                         emit (stream9_send_bytes_signal,stream9_offset);

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
    std::vector<int> missing;
    int diff = arr[0] - 0;
    int j = 1;
    missing.push_back(arr[0] - 1) ;
    for (int i = 0; i < N; i++) {
        if (arr[i] - i != diff) {
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
    for (int i = 0 ; i < size; i++) {
        if (new_missing[i+1] == new_missing[i] - 1)
            current_gap++;

        else {
            range* current_range =new range;
            current_range->ACK_range_length=new_missing[i] - new_missing[i + 1] - 1;
            current_range->gap=current_gap;
            ack_ranges_arr->push_back(current_range);
            current_gap = 1;
        }
    }

    range* last_range=new range;
    last_range->ACK_range_length=0;
    last_range->gap=new_missing[size] - rcv_next + 1;
    ack_ranges_arr->push_back(last_range);

    ack_range_info* ack_range_ = new ack_range_info;
    ack_range_->ack_range_arr=ack_ranges_arr;
    ack_range_->ACK_range_count=ack_ranges_arr->size();
    ack_range_->arr_size=ack_ranges_arr->size();
    ack_range_->first_ack_range=first_ack_range;
    delete new_missing;
    return ack_range_;
}


void ConnectionManager::socketErrorArrived(UdpSocket *socket, Indication *indication) {
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}


void ConnectionManager::socketClosed(UdpSocket *socket) {

}


void ConnectionManager::initialize() {
    localPort = par("localPort");
    destPort = par("destPort");
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
    snd_wnd_Signal = registerSignal("snd_wnd");
    bytes_sent_signal = registerSignal("num_bytes_sent");
    bytes_sent_with_ret_signal = registerSignal("bytes_sent_with_ret");
    rtt_signal = registerSignal("rtt");
    current_new_sent_bytes_signal = registerSignal("current_new_sent_bytes");
    current_total_sent_bytes_signal = registerSignal("current_total_sent_bytes");
    current_new_sent_bytes_signal_for_long = registerSignal("current_new_sent_bytes_for_long");
    current_total_sent_bytes_signal_for_long = registerSignal("current_total_sent_bytes_for_long");

    total_bytes_in_curr_send_signal = registerSignal("total_bytes_in_curr_send");
    new_bytes_in_curr_send_signal = registerSignal("new_bytes_in_curr_send");
    stream0_send_bytes_signal = registerSignal("stream0");
    stream1_send_bytes_signal = registerSignal("stream1");
    stream2_send_bytes_signal = registerSignal("stream2");
    stream3_send_bytes_signal = registerSignal("stream3");
    stream4_send_bytes_signal = registerSignal("stream4");
    stream5_send_bytes_signal = registerSignal("stream5");
    stream6_send_bytes_signal = registerSignal("stream6");
    stream7_send_bytes_signal = registerSignal("stream7");
    stream8_send_bytes_signal = registerSignal("stream8");
    stream9_send_bytes_signal = registerSignal("stream9");
    latency_signal = registerSignal("latency1");
    latency_signal_second = registerSignal("latency2");
    latency_signal_third  = registerSignal("latency3");
    latency_signal_fourth = registerSignal("latency4");
}


void ConnectionManager::handleMessage(cMessage *msg) {
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
                 if ((*it)->getDestID() == dest_id){
                     dynamic_cast<QuicConnectionClient*>(*it)->processRexmitTimer(); //update congestion control after timeout
                     Packet* new_packet = (dynamic_cast<QuicConnectionClient*>(*it))->createRetransmitPacket(rto_expired_packet, true);
                     dynamic_cast<QuicConnectionClient*>(*it)->createCopyPacket(new_packet);
                     // setRTO on copy packet
                     auto header_new = new_packet->peekAtFront<QuicPacketHeader>();
                     int packet_number = header_new->getPacket_number();
                     // if packet no longer in send_not_ACKed queue -> need to change
                     Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacketInSendQueue(packet_number);
                     copy_packet->setKind(RTO_EXPIRED_EVENT);
                     simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->getRto();
                     scheduleAt(simTime()+RTO,copy_packet);
                     sendPacket(new_packet,(*it)->getDestAddress());
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
                 if ((*it)->getDestID() == dest_id){
                     Packet* copy_packet = rto_expired_packet->dup();
                     // setRTO on copy packet
                     copy_packet->setKind(RTO_INITIAL_EXPIRED_EVENT);
                     simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->getRto();
                     scheduleAt(simTime()+RTO,rto_expired_packet);
                     sendPacket(copy_packet,(*it)->getDestAddress());
                     EV<< "send retransmit of initial packet " << endl;
                 }
            }
        }

        else if (msg->getKind() == ACK_EXPIRED_EVENT) {
            timer_msg* ACK_timer_msg = dynamic_cast<timer_msg*>(msg);
            int dest_ID_from_peer = ACK_timer_msg->getDest_connection_ID();
            EV << "ACK timer EXPIRED!!!!!!!!!!!!!!"<< endl;
            for (std::list<QuicConnection*>::iterator it =
                    connections->begin(); it != connections->end(); ++it) {
                if ((*it)->getSourceID() == dest_ID_from_peer) {
                    // create ACK packet
                    char msgName[32];
                    sprintf(msgName, "QUIC packet ACK");
                    Packet *ack = new Packet(msgName);
                    int dest_ID = (*it)->getDestID();

                    // create short header
                    unsigned int dest_ID_len = (*it)->calcSizeInBytes(dest_ID);
                    auto QuicHeader_ = makeShared<QuicShortHeader>();
                    QuicHeader_->setHeader_form(b(0));
                    QuicHeader_->setDest_connection_ID(dest_ID);
                    int header_length = SHORT_HEADER_BASE_LENGTH + dest_ID_len;
                    QuicHeader_->setChunkLength(B(header_length));
                    ack->insertAtFront(QuicHeader_);

                    // create copy of ACK frame
                    auto ACK_frame_to_send = makeShared<ACKFrame>();
                    ACK_frame_to_send->setFrame_type(2);
                    std::list<int> not_acked_list=(dynamic_cast<QuicConnectionServer*>(*it))->getNotAckedList();
                    int ack_range_count=0;
                    if (!not_acked_list.empty()){// we need to send out of order ACK
                        int rcv_next=(dynamic_cast<QuicConnectionServer*>(*it))->getRcvNext();
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
                        ack_range_info* ack_range_info_= createAckRange(arr,list_size,smallest, largest,rcv_next);
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
                        dynamic_cast<QuicConnectionServer*>(*it)->setLargestWithRcvNext();

                    }

                    else {
                        int largest= dynamic_cast<QuicConnectionServer*>(*it)->getLargestInOrder();
                        int prev_largest=dynamic_cast<QuicConnectionServer*>(*it)->getCurrLargest();
                        ACK_frame_to_send->setFirst_ACK_range(largest-prev_largest-1); //no out of order ack,first_range_count=0;
                        ACK_frame_to_send->setLargest_acknowledged(largest);
                        dynamic_cast<QuicConnectionServer*>(*it)->setCurrLargest(largest);
                    }

                    ACK_frame_to_send->setChunkLength(B(sizeof(int)*(4+2*ack_range_count)));
                    ack->insertAtBack(ACK_frame_to_send);
                    // current_Ack_Frame;
                    sendPacket(ack,(*it)->getDestAddress());
                }
            }
        }

        else if (msg->getKind() == HANDSHAKE_TIMER_EVENT){
            timer_msg* handshake_timer_msg = dynamic_cast<timer_msg*>(msg);
            int src_id = handshake_timer_msg->getDest_connection_ID();
            for (std::list<QuicConnection*>::iterator it =
                            connections->begin(); it != connections->end(); ++it) {
                if ((*it)->getSourceID() == src_id) {
                    Packet *handshake_packet = (dynamic_cast<QuicConnectionClient*>(*it))->ProcessInitiateHandshake(src_id);
                    Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findInitialPacket();
                    copy_packet->setKind(RTO_INITIAL_EXPIRED_EVENT);
                    simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->getRto();
                    scheduleAt(simTime()+RTO,copy_packet);

                    if ((dynamic_cast<QuicConnectionClient*>(*it))->getReconnect()) {
                        sendPacket(handshake_packet,(*it)->getDestAddress());
                        for (std::vector<old_transport_parameters*>::iterator it_trans_params =
                                old_trans_parameters.begin(); it_trans_params != old_trans_parameters.end(); ++it_trans_params) {
                                if ((*it)->getDestAddress() == (*it_trans_params)->destination) {
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
                            Packet* copy_packet = (dynamic_cast<QuicConnectionClient*>(*it))->findPacketInSendQueue(packet_number);
                            copy_packet->setKind(RTO_EXPIRED_EVENT);
                            simtime_t RTO = dynamic_cast<QuicConnectionClient*>(*it)->getRto();
                            scheduleAt(simTime()+RTO,copy_packet);
                            sendPacket(curr_packet_to_send,(*it)->getDestAddress());
                        }

                        packets_to_send->clear();

                        // record bytes_sent
                        int bytes_sent = (dynamic_cast<QuicConnectionClient*>(*it))->getNumBytesSent();
                        int bytes_sent_with_ret = (dynamic_cast<QuicConnectionClient*>(*it))->getNumBytesSentWithRet();
                        emit(bytes_sent_signal, bytes_sent);
                        emit(bytes_sent_with_ret_signal,bytes_sent_with_ret);

                        int stream0_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(0);
                        int stream1_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(1);
                        int stream2_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(2);
//                        int stream3_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(3);
//                        int stream4_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(4);
//                        int stream5_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(5);
//                        int stream6_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(6);
//                        int stream7_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(7);
//                        int stream8_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(8);
//                        int stream9_offset=(dynamic_cast<QuicConnectionClient*>(*it))->getStreamBytesSent(9);
                        emit (stream0_send_bytes_signal,stream0_offset);
                        emit (stream1_send_bytes_signal,stream1_offset);
                        emit (stream2_send_bytes_signal,stream2_offset);
//                        emit (stream3_send_bytes_signal,stream3_offset);
//                        emit (stream4_send_bytes_signal,stream4_offset);
//                        emit (stream5_send_bytes_signal,stream5_offset);
//                        emit (stream6_send_bytes_signal,stream6_offset);
//                        emit (stream7_send_bytes_signal,stream7_offset);
//                        emit (stream8_send_bytes_signal,stream8_offset);
//                        emit (stream9_send_bytes_signal,stream9_offset);


                        // record window change
                        int send_window = (dynamic_cast<QuicConnectionClient*>(*it))->getSendWindow();
                        emit(snd_wnd_Signal, send_window);
                        break;
                   }
                    else
                        sendPacket(handshake_packet,(*it)->getDestAddress());
                }
            }
        }

        else if (msg->getKind() == UPDATE_THROUGHPUT) {
            timer_msg* thr_timer_msg = dynamic_cast<timer_msg*>(msg);
            int dest_ID_from_peer = thr_timer_msg->getDest_connection_ID();
            for (std::list<QuicConnection*>::iterator it =
                    connections->begin(); it != connections->end(); ++it) {
                if ((*it)->getSourceID() == dest_ID_from_peer) {
                    // bytes sent for Throughput
                    int current_total_sent_bytes = (dynamic_cast<QuicConnectionClient*>(*it))->getCurrentBytesSent(true);
                    int current_new_sent_bytes = (dynamic_cast<QuicConnectionClient*>(*it))->getCurrentBytesSent(false);


                    emit(current_total_sent_bytes_signal, current_total_sent_bytes * 10);
                    emit(current_new_sent_bytes_signal, current_new_sent_bytes * 10);
                    throughput_timer->setKind(UPDATE_THROUGHPUT);
                    scheduleAt(simTime()+THROUGHPUT_GAP,throughput_timer);
                }
            }
        }
        else if (msg->getKind() == UPDATE_THROUGHPUT_LONG) {
            timer_msg* thr_timer_msg = dynamic_cast<timer_msg*>(msg);
            int dest_ID_from_peer = thr_timer_msg->getDest_connection_ID();
            for (std::list<QuicConnection*>::iterator it =
                    connections->begin(); it != connections->end(); ++it) {
                if ((*it)->getSourceID() == dest_ID_from_peer) {
                    // bytes sent for Throughput
                    int current_total_sent_bytes_long = (dynamic_cast<QuicConnectionClient*>(*it))->getCurrentBytesSentLong(true);
                    int current_new_sent_bytes_long = (dynamic_cast<QuicConnectionClient*>(*it))->getCurrentBytesSentLong(false);

                    emit(current_total_sent_bytes_signal_for_long, current_total_sent_bytes_long);
                    emit(current_new_sent_bytes_signal_for_long, current_new_sent_bytes_long);
                    throughput_timer_long->setKind(UPDATE_THROUGHPUT_LONG);
                    scheduleAt(simTime()+LONG_THROUGHPUT_GAP,throughput_timer_long);
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
            connection->setSourceID(src_ID_);

            // bytes sent for Throughput
            emit(current_total_sent_bytes_signal, 0);
            emit(current_new_sent_bytes_signal, 0);
            throughput_timer->setDest_connection_ID(src_ID_);
            throughput_timer->setKind(UPDATE_THROUGHPUT);
            scheduleAt(simTime()+THROUGHPUT_GAP,throughput_timer);

            // bytes sent for long Throughput
            emit(current_total_sent_bytes_signal_for_long, 0);
            emit(current_new_sent_bytes_signal_for_long, 0);
            throughput_timer_long->setDest_connection_ID(src_ID_);
            throughput_timer_long->setKind(UPDATE_THROUGHPUT_LONG);
            scheduleAt(simTime()+LONG_THROUGHPUT_GAP,throughput_timer_long);

            timer_msg* handshake_timer = new timer_msg ("handshake timer");
            handshake_timer->setKind(HANDSHAKE_TIMER_EVENT);
            handshake_timer->setDest_connection_ID(src_ID_);
            if (reconnect)
                scheduleAt(simTime(),handshake_timer);

            else
                scheduleAt(simTime()+DELAY_TIME,handshake_timer);

            delete connection_data;
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
                    if ((*it)->getDestAddress()==destination) {
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
    packet->setTimestamp(simTime());
    socket.sendTo(packet, destAddr, destPort);
}


bool ConnectionManager::isIDAvailable(int dest_ID) {
    for (std::list<QuicConnection*>::iterator it = connections->begin();
            it != connections->end(); ++it) {
        if ((*it)->getSourceID() == dest_ID)
            return false;
    }
    return true;
}


} /* namespace inet */
