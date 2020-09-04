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
#include "QuicNewReno.h"
#include "QuicData_m.h"
#include "QuicPacketHeader_m.h"
#include "QuicHandShakeData_m.h"
#include "MaxStreamData_m.h"
#include "StreamsData.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/Packet.h"
#include <omnetpp.h>
#include "inet/common/ModuleAccess.h"


#ifndef INET_APPLICATIONS_QUICAPP_QuicConnection_H_
#define INET_APPLICATIONS_QUICAPP_QuicConnection_H_

namespace inet {

class QuicStreamArr;
class QuicSendQueue;
class QuicRecieveQueue;
class QuicNewRENO;

enum QuicState {
    QUIC_S_CLIENT_INITIATE_HANDSHAKE = 0,
    QUIC_S_SERVER_PROCESS_HANDSHAKE = FSM_Steady(1),
    QUIC_S_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE = FSM_Steady(2),
    QUIC_S_SERVER_WAIT_FOR_DATA =FSM_Steady(3),
    QUIC_S_SEND = FSM_Steady(4),
    QUIC_S_NEW_CONNECTION = FSM_Steady(5),
    QUIC_S_RECONNECTION = FSM_Steady(6),
    QUIC_S_LISTEN = FSM_Steady(7),
    QUIC_S_CONNECTION_TERM = FSM_Steady(8),
};

enum QuicEventCode {
    QUIC_E_CLIENT_INITIATE_HANDSHAKE = 0,
    QUIC_E_SERVER_PROCESS_HANDSHAKE,
    QUIC_E_CLIENT_WAIT_FOR_HANDSHAKE_RESPONSE,
    QUIC_E_SERVER_WAIT_FOR_DATA,
    QUIC_E_SEND,
    QUIC_E_NEW_CONNECTION,
    QUIC_E_RECONNECTION,
    QUIC_E_LISTEN,
    QUIC_E_CONNECTION_TERM,
};

/*
 * things to add:
 * - receive queue because the reciver need to get the bytes in order.
 * -
 *
 */

class QuicConnection { //public cSimpleModule, public UdpSocket::ICallback{
public:
    QuicConnection(L3Address destination);
    QuicConnection(int* connection_data, int connection_data_size,L3Address destination);

    virtual ~QuicConnection();
    Packet* createQuicDataPacket(StreamsData* streams_data);
    void sendPacket(Packet *packet);
    StreamsData* CreateSendData(int max_payload, int connection_window);
    void recievePacket(std::vector<stream_frame*> accepted_frames);
    void AddNewStream(int stream_size,int index);
    bool CloseStream(int stream_id);

    void performStateTransition(const QuicEventCode &event);
    Packet* ProcessEvent(const QuicEventCode &event,Packet* packet);
    Packet* ActivateFsm(Packet* packet);
    Packet* ProcessInitiateHandshake(Packet* packet);
    Packet* ServerProcessHandshake(Packet* packet);
    Packet* ProcessClientHandshakeResponse(Packet* packet);
    Packet* ProcessServerWaitData(Packet* packet);
    Packet* ProcessClientSend(Packet* packet);

    //void ProcessNewConnection(cMessage *msg);
    //void ProcessReconnection(cMessage *msg);
    //Packet* ProcessConnectionSend(cMessage *msg);
    //void ProcessConnectionListen(cMessage *msg);
    //void processConnectionTerm(cMessage *msg);
    int GetFramesNumber();
    int GetDataSize();
    void SetFramesNumber(int frames_number);
    void SetDataSize(int data_size);
    int GetSourceID();
    void SetSourceID(int source_ID);
    int GetDestID();
    void SetDestID(int dest_ID);
    bool SendMaxDataPacket();
    int GetTotalConsumedBytes();
    int GetConnectionsRecieveWindow();
    int GetConnectionsRecieveOffset();
    int GetConnectionMaxWindow();
    int GetMaxOffset();
    void setConnectionsRecieveOffset(int offset);
    void setConnectionsRecieveWindow(int window_size);
    //void setClientNumber(int client_number);
    L3Address GetDestAddress();
    Packet* RemovePacketFromQueue(int packet_number);
    void updateFlowControl(Packet* acked_packet);
    void updateCongestionWindow();


    void updateMaxStreamData(int stream_id, int max_stream_data_offset);

    //virtual void initialize() override;
    //virtual void handleMessage(cMessage *msg) override;
    std::vector<Packet*> getMaxStreamDataPackets();

    QuicEventCode preanalyseAppCommandEvent(int commandCode);
    int GetEventKind();
    bool GetFirstConnection() {
        return this->first_connection;
    }

    void UpdateRtt(simtime_t acked_time){
        uint32 acked_time_int = this->congestion_alg->convertSimtimeToTS(acked_time);
        this->congestion_alg->rttMeasurementCompleteUsingTS(acked_time_int);
    }

  //  void moveDataToSendQueue(int bytes_num);
  //  void ProcessInitialClientData(int total_bytes_to_send);

//    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
//    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
//    virtual void socketClosed(UdpSocket *socket) override;
//    virtual L3Address chooseDestAddr();




//    void SetToFirstConnection(){this->first_connection=true;
//        this->event->setKind(QUIC_E_NEW_CONNECTION);
//    }
//    void SetToReConnection(){this->first_connection=false;
//        this->event->setKind(QUIC_E_RECONNECTION);
//    }
protected:
    int connection_source_ID;
    int connection_dest_ID;
    int module_number_to_send;
    // state
//     UdpSocket socket;
//     L3Address destAddr;

//     int localPort = -1, destPort = -1;
//    std::vector<L3Address> destAddresses;
//    int destAddrRNG = -1;

// std::vector<std::string> destAddressStr;


    QuicStreamArr *stream_arr; //the class that represent the entire data of the data that was sent
    //QuicConnectionSendQueue *send_queue; // this queue suppose to hold packets not acked yet
    std::list<Packet*> send_not_ACKED_queue;
    std::list<int> receive_not_ACKED_queue;
    L3Address destination;


    //   QuicRecieveQueue *recieve_queue;
    int packet_counter; // counter to assign each packet header a unique packet number
    int num_packets_sent;
    int num_packets_recieved;
    uint32 total_bytes_to_send; // how many bytes in total we want to transfer in this connection

    cFSM fsm; // QUIC state machine
    //for start we have a flag for first connection which changes in the first time of the connection.
    // maybe in the future we will change it and really connect between client and server. (pass some flag about first connection between them)
    bool first_connection;

    int curr_frames_number; // the number of frames in each stream that the connection can currently send in a single packet.
    int curr_data_size; // the number of bytes that can currently be sent in a single packet
    cMessage *event = nullptr;
    cMessage *start_fsm;

    //flow control server side parameters
    int connection_max_flow_control_window_size; // constant
    int connection_flow_control_recieve_offset; //
    int connection_flow_control_recieve_window; // flow_control_recieve_offset - highest_recieved_byte_offset


    //flow control client side parameters

    int inital_stream_window;
    int max_payload;

    std::vector<Packet*> max_stream_data_packets_;

    int rcv_next;
    int last_rcvd_ACK;
    int dup_ACKS;
    int snd_cwnd; //congestion window
    int ssthresh; // slow start threshold

    QuicNewReno* congestion_alg;

};

//Define_Module(QuicConnection);

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QuicConnection_H_ */
