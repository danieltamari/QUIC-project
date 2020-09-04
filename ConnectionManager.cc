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


#define SENDER 1
#define RECEIVER 2

#define CHECK 12


namespace inet {

ConnectionManager::ConnectionManager() {

}

ConnectionManager::~ConnectionManager() {
    // TODO Auto-generated destructor stub
}

void ConnectionManager::socketDataArrived(UdpSocket *socket, Packet *packet) {
    // process incoming packet
    auto header = packet->peekAtFront<QuicPacketHeader>();

    int packet_type = header->getPacket_type();
    int src_ID_from_peer = header->getSrc_connectionID();
    int dest_ID_from_peer = header->getDest_connectionID();

    switch (packet_type) {
    case HANDSHAKE: { // HANDSHAKE PACKET (in server)

        L3Address srcAddr = packet->getTag<L3AddressInd>()->getSrcAddress();
        QuicConnection connection = QuicConnection(srcAddr); // create new connection at server's side
        if (isIDAvailable(src_ID_from_peer)) {
            connection.SetDestID(src_ID_from_peer);
        } else {
            // get different ID and update on connection.SetDestID
        }
        //connection.SetSourceID(0);
        connection.SetSourceID(dest_ID_from_peer);
        int index = connections.size();
        this->connections.push_back(connection);

        Packet *handshake__response_packet = connections[index].ActivateFsm(packet);
        sendPacket(handshake__response_packet,connection.GetDestAddress());
        break;
    }

    case HANDSHAKE_RESPONSE: { // HANDSHAKE RESPONSE PACKET (in client)
        for (std::vector<QuicConnection>::iterator it =
                this->connections.begin(); it != connections.end(); ++it) {
            if (it->GetDestID() == src_ID_from_peer) {
                it->SetSourceID(dest_ID_from_peer);
                it->SetDestID(src_ID_from_peer);
                Packet *first_data_packet = it->ActivateFsm(packet);
                sendPacket(first_data_packet,it->GetDestAddress());

                break;
            }
        }
        break;

    }

    case FIRST_STREAMS_DATA: { //DATA PACKET
        for (std::vector<QuicConnection>::iterator it =
                this->connections.begin(); it != connections.end(); ++it) {
            if (it->GetDestID() == src_ID_from_peer) {
                Packet *ACK_packet = it->ActivateFsm(packet);
                sendPacket(ACK_packet,it->GetDestAddress());
                std::vector<Packet*> connection_max_data_vector = it->getMaxStreamDataPackets();
                for (std::vector<Packet*>::iterator it_packet =
                        connection_max_data_vector.begin(); it_packet != connection_max_data_vector.end(); ++it_packet) {
                Packet* send_max_data_packet = *it_packet;
                sendPacket(send_max_data_packet,it->GetDestAddress());
                }
                for (std::vector<Packet*>::iterator it_max=
                        connection_max_data_vector.begin(); it_max != connection_max_data_vector.end(); ++it_max) {
                    connection_max_data_vector.pop_back();
                }
                int total_connection_consumed_bytes=it->GetTotalConsumedBytes();
                int connection_flow_control_recieve_offset = it->GetConnectionsRecieveOffset();
                int connection_max_flow_control_window_size = it->GetConnectionMaxWindow();
                int max_connection_offset=it->GetMaxOffset();


                if (connection_flow_control_recieve_offset-total_connection_consumed_bytes<connection_max_flow_control_window_size/2){
                    // create max_data packet
                    char msgName[32];
                    sprintf(msgName, "MAX DATA packet");
                    Packet *max_data_packet = new Packet(msgName);
                    int src_ID = it->GetSourceID();
                    int dest_ID = it->GetDestID();
                    auto QuicHeader = makeShared<QuicPacketHeader>();
                    QuicHeader->setDest_connectionID(dest_ID);
                    QuicHeader->setSrc_connectionID(src_ID);
                    QuicHeader->setPacket_type(5);
                    QuicHeader->setChunkLength(B(sizeof(int)*4));
                    max_data_packet->insertAtFront(QuicHeader);
                    const auto &payload = makeShared<MaxData>();
                    int max_offset = total_connection_consumed_bytes+connection_max_flow_control_window_size;
                    it->setConnectionsRecieveOffset(max_offset);
                    int window_size = max_offset-max_connection_offset;
                    it->setConnectionsRecieveWindow(window_size);
                    payload->setMaximum_Data(max_offset);
                    payload->setChunkLength(B(sizeof(int)*1));
                    max_data_packet->insertAtBack(payload);
                    //sendPacket(max_data_packet);
                }
                break;
            }
        }
        break;
    }

    case ACK_PACKET: { //ACK
        for (std::vector<QuicConnection>::iterator it =
                 this->connections.begin(); it != connections.end(); ++it) {
             if (it->GetDestID() == src_ID_from_peer){
                 int packet_number = header->getPacket_number();
                 Packet* acked_packet=it->RemovePacketFromQueue(packet_number);
                 it->UpdateRtt(acked_packet->getTimestamp());//update rtt meassurment
                 it->updateFlowControl(acked_packet);
                 Packet *data_packet = it->ActivateFsm(packet);
                 sendPacket(data_packet,it->GetDestAddress());
                 break;
             }
        }

        break;
    }

    case MAX_STREAM_DATA: { //MAX_STREAM_DATA
        auto data = packet->peekData<MaxStreamData>();
        int stream_id = data->getStream_ID();
        int max_stream_data_offset = data->getMaximum_Stream_Data();
        for (std::vector<QuicConnection>::iterator it =
                this->connections.begin(); it != connections.end(); ++it) {
            if (it->GetDestID() == src_ID_from_peer){
                it->updateMaxStreamData(stream_id, max_stream_data_offset);
                break;
            }
        }
        break;
    }

    case MAX_DATA: { //MAX_DATA
        auto data = packet->peekData<MaxData>();
        int max_data_offset = data->getMaximum_Data();
        for (std::vector<QuicConnection>::iterator it =
                 this->connections.begin(); it != connections.end(); ++it) {
             if (it->GetDestID() == src_ID_from_peer){
                 it->setConnectionsRecieveOffset(max_data_offset);
                 int max_connection_offset = it->GetMaxOffset();
                 int window_size = max_data_offset-max_connection_offset;
                 it->setConnectionsRecieveWindow(window_size);
                 break;
             }
         }
        break;
    }


    // active FSM
    }
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
    else {
        connectToUDPSocket();
        if (msg->getKind() == SENDER) {
            Packet *packet = check_and_cast<Packet*>(msg);
          //  uint32 total_bytes = (packet->getByteLength());
            auto packet_data = packet->peekData<connection_config_data>();
            int connection_data_size = packet_data->getConnection_dataArraySize();
            int* connection_data = new int [connection_data_size];
            for (int i=0; i<connection_data_size; i++) {
                connection_data[i] = packet_data->getConnection_data(i);
            }
            const char* connectAddress=packet_data->getConnectAddress();
            //int my_client_number=packet_data->getMy_client_number();

            L3Address destination;
            L3AddressResolver().tryResolve(connectAddress, destination);

            //if (destination.isUnspecified())
            //    EV_ERROR << "Connecting to " << connectAddress << " port=" << connectPort << ": cannot resolve destination address\n";
            //else
            //    EV_INFO << "Connecting to " << connectAddress << "(" << destination << ") port=" << connectPort << endl;

            int connection_index = AddNewConnection(connection_data, connection_data_size,destination);

            //Packet* client_number_pkt = new Packet ("client_number");
            //client_number_pkt->setKind(my_client_number);

            Packet *handshake_packet =
                    connections[connection_index].ActivateFsm(packet); //activate fsm always return a packet which type is set by the current state: handshake,data, etc....
            //handshake_packet->setKind(my_client_number);

            //handshake_packet->setControlInfo(client_number);
            sendPacket(handshake_packet,destination);
        }
    }
}

int ConnectionManager::AddNewConnection(int* connection_data, int connection_data_size,L3Address destination) {
    QuicConnection connection = QuicConnection(connection_data, connection_data_size,destination);

    int index = connections.size();
    this->connections.push_back(connection);
    return index;
}

void ConnectionManager::connectToUDPSocket() {

    socket.bind(localPort);
}

void ConnectionManager::sendPacket(Packet *packet,L3Address destAddr) {

    //L3Address destAddr = chooseDestAddr(module_number);
    socket.sendTo(packet, destAddr, this->destPort);
}

bool ConnectionManager::isIDAvailable(int src_ID) {
    for (std::vector<QuicConnection>::iterator it = this->connections.begin();
            it != connections.end(); ++it) {
        if (it->GetDestID() == src_ID)
            return false;
    }
    return true;
}

} /* namespace inet */
