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


#ifndef INET_APPLICATIONS_QUICAPP_QuicConnection_H_
#define INET_APPLICATIONS_QUICAPP_QuicConnection_H_

namespace inet {

class QuicStreamArr;
class QuicSendQueue;
class QuicRecieveQueue;

enum QuicState {
    QUIC_S_INIT = 0,
    QUIC_S_NEW_CONNECTION = FSM_Steady(1),
    QUIC_S_RECONNECTION = FSM_Steady(2),
    QUIC_S_CONNECTION_EST = FSM_Steady(3),
    QUIC_S_CONNECTION_TERM = FSM_Steady(4),
};

enum QuicEventCode {
    QUIC_E_INIT = 0,
    QUIC_E_NEW_CONNECTION,
    QUIC_E_RECONNECTION,
    QUIC_E_CONNECTION_EST,
    QUIC_E_CONNECTION_TERM,
};

/*
 * things to add:
 * - receive queue because the reciver need to get the bytes in order.
 * -
 *
 */

class QuicConnection: public cSimpleModule, public UdpSocket::ICallback{
public:
    QuicConnection();
    QuicConnection(int max_streams_num, int localPort, int destPort,
            int total_bytes/*, L3Address IP_address*/);
    virtual ~QuicConnection();
    Packet* createQuicPacket(const StreamsData sterams_data);
    void sendPacket(Packet *packet);
    StreamsData* CreateSendData(int frames_number, int total_bytes_to_send);
    void recievePacket(Packet *packet);
    int AddNewStream(int max_bytes, int quanta, int total_bytes);
    bool CloseStream(int stream_id);

    bool performStateTransition(const QuicEventCode &event);
    bool ProcessEvent(const QuicEventCode &event, cMessage *msg);
    void ProcessInitState(cMessage *msg);
    void ProcessNewConnection(cMessage *msg);
    void ProcessReconnection(cMessage *msg);
    void ProcessConnectionEst(cMessage *msg);
    void processConnectionTerm(cMessage *msg);
    int GetFramesNumber();
    int GetDataSize();
    void SetFramesNumber(int frames_number);
    void SetDataSize(int data_size);

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    QuicEventCode preanalyseAppCommandEvent(int commandCode);
    int GetEventKind();
    bool GetFirstConnection() {
        return this->first_connection;
    }

    void moveDataToSendQueue(int bytes_num);
    void ProcessInitialClientData(int total_bytes_to_send);

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
    virtual L3Address chooseDestAddr();




//    void SetToFirstConnection(){this->first_connection=true;
//        this->event->setKind(QUIC_E_NEW_CONNECTION);
//    }
//    void SetToReConnection(){this->first_connection=false;
//        this->event->setKind(QUIC_E_RECONNECTION);
//    }
protected:
    // state
     UdpSocket socket;
     L3Address destAddr;


    //int local_port=1;
    //int dest_port=1; // for socket.sendTo
     int localPort = -1, destPort = -1;


    //L3Address destIPaddress; // for socket.sendTo
    std::vector<L3Address> destAddresses;
    //ChooseDestAddrMode chooseDestAddrMode = static_cast<ChooseDestAddrMode>(0);
    int destAddrRNG = -1;

    // std::vector<std::string> destAddressStr;


    QuicStreamArr *stream_arr; //the class that represent the entire data of the data that was sent
    QuicSendQueue *send_queue; // this queue suppose to hold bytes when all the streams are full
    QuicRecieveQueue *recieve_queue;
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


};

Define_Module(QuicConnection);

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QuicConnection_H_ */
