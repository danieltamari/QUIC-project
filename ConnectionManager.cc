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

#define SENDER 1
#define RECEIVER 2

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
    case HANDSHAKE: { // HANDSHAKE PACKET
        QuicConnection connection = QuicConnection(); // create new connection at server's side
        if (isIDAvailable(src_ID_from_peer)) {
            connection.SetDestID(src_ID_from_peer);
        } else {
            // get different ID and update on connection.SetDestID
        }
        //connection.SetSourceID(0);
        connection.SetSourceID(dest_ID_from_peer);
        int index = connections.size();
        this->connections.push_back(connection);

        Packet *handshake__response_packet = connections[index].ActivateFsm();
        sendPacket(handshake__response_packet);
    }
    case HANDSHAKE_RESPONSE: { // HANDSHAKE RESPONSE PACKET
        for (std::vector<QuicConnection>::iterator it =
                this->connections.begin(); it != connections.end(); ++it) {
            if (it->GetDestID() == src_ID_from_peer) {
                it->SetSourceID(dest_ID_from_peer);
                it->SetDestID(src_ID_from_peer);
                break;
            }
        }
    }
    }
    // active FSM
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

L3Address ConnectionManager::chooseDestAddr() {
    if (destAddresses.size() == 1)
        return destAddresses[0];
    int k = getRNG(destAddrRNG)->intRand(destAddresses.size());
    return destAddresses[k];

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
            uint32 total_bytes = (packet->getByteLength());
            int connection_index = AddNewConnection(total_bytes);
            Packet *handshake_packet =
                    connections[connection_index].ActivateFsm(); //activate fsm always return a packet which type is set by the current state: handshake,data, etc....
            sendPacket(handshake_packet);
        }
    }
}

int ConnectionManager::AddNewConnection(uint32 data_size) {
    QuicConnection connection = QuicConnection(data_size);
    int index = connections.size();
    this->connections.push_back(connection);
    return index;
}

void ConnectionManager::connectToUDPSocket() {

    socket.bind(localPort);
    destAddrRNG = par("destAddrRNG");

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;
    //bool excludeLocalDestAddresses = par("excludeLocalDestAddresses");

    //IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);

    while ((token = tokenizer.nextToken()) != nullptr) {
        if (strstr(token, "Broadcast") != nullptr)
            destAddresses.push_back(Ipv4Address::ALLONES_ADDRESS);
        else {
            L3Address addr = L3AddressResolver().resolve(token);
            //      if (excludeLocalDestAddresses && ift && ift->isLocalAddress(addr))
            //         continue;
            destAddresses.push_back(addr);
        }
    }
}

void ConnectionManager::sendPacket(Packet *packet) {

    L3Address destAddr = chooseDestAddr();
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
