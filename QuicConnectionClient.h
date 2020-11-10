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
#include "headers_and_frames/QuicPacketHeader_m.h"
#include "headers_and_frames/QuicLongHeader_m.h"
#include "headers_and_frames/QuicShortHeader_m.h"
#include "headers_and_frames/QuicHandShakeData_m.h"
#include "headers_and_frames/ACKFrame_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"


#ifndef INET_APPLICATIONS_QUICAPP_QuicConnection_H_
#define INET_APPLICATIONS_QUICAPP_QuicConnection_H_

namespace inet {

class QuicConnectionClient : public QuicConnection { //public cSimpleModule, public UdpSocket::ICallback{
public:

    QuicConnectionClient();
    QuicConnectionClient(int* connection_data, int connection_data_size,L3Address destination_,bool reconnect_);
    virtual ~QuicConnectionClient();
    void AddNewStream(int stream_size,int index);
    Packet* ProcessInitiateHandshake( int src_ID_);
    void ProcessClientHandshakeResponse(Packet* packet);
    void ProcessClientSend();
    void ProcessClientZeroRtt(int connection_window_size, int max_payload_, int stream_window_size);
    void ProcessClientACK(packet_rcv_type* acked_packet_arr, int total_acked);
    Packet* createQuicDataPacket(std::vector<IntrusivePtr<StreamFrame>>* frames_to_send, int total_size, bool one_RTT);
    void createCopyPacket(Packet* original_packet);
    Packet* createRetransmitPacket(Packet* packet_to_remove, bool immedidate_retransmit);
    void updateFlowControl(Packet* acked_packet);
    void updateStreamFlowControl (int  stream_id,int flow_control_window);
    void updateRtt(simtime_t acked_time);
    void updateCongestionAlgo(std::vector<int>* lost_packets_numbers, int largest);
    void updateStreamInfo(Packet* copy_of_ACKED_packet);
    void updateLostPackets(int largest);
    void processRexmitTimer();
    Packet* findPacketInSendQueue(int packet_number);
    Packet* RemovePacketFromQueue(int packet_number);
    Packet* findInitialPacket();
    std::list<Packet*>* getPacketsToSend();
    std::list<Packet*>* getLostPackets();
    std::list<Packet*>* getPacketsToCancel(int cancel_RTO_before_number);
    int getSendWindow();
    int getNumBytesSent();
    int getNumBytesSentWithRet();
    int getCurrentBytesSent(bool with_ret);
    int getStreamBytesSent(int stream_id);
    simtime_t getRto();
    simtime_t getRtt();
    bool getReconnect();
    bool getEndConnection();
    int getConnectionWindow();
    int getMaxPayload();
    int getStreamWindow();
    int getStreamNumber();
    void setStreamNumber(int new_stream_number);
    int getTotalBytesInCurrSend();
    int getNewBytesInCurrSend();
    int getCurrentBytesSentLong(bool with_ret);


protected:
    int packet_counter; // counter to assign each packet header a unique packet number
    int send_una; // sent and unacknowledged bytes so far
    QuicSendQueue* send_queue;
    std::list<Packet*>* ACKED_out_of_order;
    std::list<Packet*>* waiting_retransmission;
    std::list<Packet*>* packets_to_send;
    bool reconnect; //new connection or reconnection
    //flow control client side parameters
    int max_payload;
    int Bytes_in_flight;
    int connection_max_flow_control_window_size; // constant
    int connection_flow_control_recieve_window; //
    int stream_flow_control_window;
    // ACK control parameters
    int last_rcvd_ACK;
    int dup_ACKS;
    int recovery_start_packet;
    QuicNewReno* congestion_alg;
    int min_packet_lost;
    int num_bytes_sent;
    int bytes_sent_with_ret;
    int current_sent_bytes;
    int current_sent_bytes_with_ret;
    int current_sent_bytes_long;
    int current_sent_bytes_with_ret_long;
    // for sent bytes signal
    int total_sent_bytes_in_curr;
    int new_sent_bytes_in_curr;
};


} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QuicConnection_H_ */
