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
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "QuicStreamArr.h"
#include "headers_and_frames/QuicPacketHeader_m.h"
#include "headers_and_frames/QuicLongHeader_m.h"
#include "headers_and_frames/QuicShortHeader_m.h"
#include "timer_msg_m.h"
#include "headers_and_frames/ACKFrame_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"
#include "QuicConnectionClient.h"
#include "QuicConnectionServer.h"
#include "headers_and_frames/connection_config_data_m.h"

#ifndef INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_
#define INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_

enum Sim_type { SENDER = 1,
                RECEIVER,
                ADDSTREAMS
               };

enum Frame_type {PADDING =0,
                  PING,
                  ACK,
                  RESET_STREAM,
                  STOP_SENDING,
                  STREAM_DATA,
                  MAX_DATA,
                  MAX_STREAM_DATA,
                  MAX_STREAMS,
                  DATA_BLOCKED,
                  STREAM_DATA_BLOCKED,
                  STREAMS_BLOCKED,
                  NEW_CONNECTION_ID,
                  CONNECTION_CLOSE,
                  HANDSHAKE_DONE
                    };

enum Packet_type {INITIAL = 0,
                  ZERO_RTT,
                  HANDSHAKE,
                  RETRY,
                    };

namespace inet {

struct ack_range_info {
    std::vector<range*>* ack_range_arr;
    int arr_size;
    int ACK_range_count;
    int first_ack_range;
};


struct old_transport_parameters {
    int connection_window_size;
    int stream_window_size;
    int max_payload;
    L3Address destination;
};


class ConnectionManager:  public cSimpleModule, public UdpSocket::ICallback{
public:
    ConnectionManager();
    virtual ~ConnectionManager();
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
    QuicConnectionClient* AddNewConnection(int* connection_data, int connection_data_size,L3Address destination,bool reconnect);
    void connectToUDPSocket();
    bool isIDAvailable(int src_ID);
    void sendPacket(Packet *packet,L3Address destAddr);
    void processPacketFrames(Packet* packet, int dest_ID_from_peer, bool new_data);
    void sendReadyPackets(QuicConnection* current_connection, bool zero_rtt);
    void setAckTimer(int dest_ID_from_peer);
    void cancelRTOonPackets(packet_rcv_type* acked_packet_arr, int total_acked, QuicConnection* current_connection);
    ack_range_info* createAckRange(int arr[],int N,int smallest,int largest,int rcv_next);
    void handleRetransmissionTimeout(cMessage *msg);
    void handleInitalTimeout(cMessage *msg);
    void handleAckTimeout(cMessage *msg);
    void handleHandshakeInit(cMessage *msg);
    void handleUpdateThroughput();
    void handleUpdateThroughputLong();



protected:
     UdpSocket socket;
     int localPort = -1, destPort = -1;
     std::vector<old_transport_parameters*> old_trans_parameters;
     std::list<QuicConnection*>* connections;
     std::map<int,timer_msg*> ACK_timer_msg_map;
     std::map<int,int> latency_connection_map;
     std::map<int,int> throughput_connection_map;
     int type = RECEIVER;
     bool connected; // connected to the udp socket
     timer_msg* throughput_timer;
     timer_msg* throughput_timer_long;
     std::set<int> connections_done;

     // parametetrs to configue server
     int max_payload;
     int init_stream_flow_control_window;
     int init_connection_flow_control_winodw;
     double max_delay;

     /// REMOVE
     int latency_index=0;
     int throughput_index=0;


private:
     simsignal_t snd_wnd_Signal;
     simsignal_t bytes_sent_signal;
     simsignal_t rtt_signal;
     simsignal_t latency_signal;
     simsignal_t latency_signal_second;
     simsignal_t latency_signal_third;
     simsignal_t latency_signal_fourth;

     simsignal_t throughput_signal;
     simsignal_t throughput_signal_second;
     simsignal_t throughput_signal_third;
     simsignal_t throughput_signal_fourth;

     simsignal_t throughput_signal_long;
     simsignal_t throughput_signal_second_long;
     simsignal_t throughput_signal_third_long;
     simsignal_t throughput_signal_fourth_long;


     simsignal_t current_new_sent_bytes_signal;
     simsignal_t current_total_sent_bytes_signal;
     simsignal_t current_new_sent_bytes_signal_for_long;
     simsignal_t current_total_sent_bytes_signal_for_long;

     simsignal_t bytes_sent_with_ret_signal;
     simsignal_t new_bytes_in_curr_send_signal;
     simsignal_t total_bytes_in_curr_send_signal;


};
Define_Module(ConnectionManager);


} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_CONNECTIONMANAGER_H_ */
