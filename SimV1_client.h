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


#ifndef INET_APPLICATIONS_QUICAPP_SimV1_client_CLIENT_H_
#define INET_APPLICATIONS_QUICAPP_SimV1_client_CLIENT_H_

#include "inet/common/INETDefs.h"
#include <omnetpp.h>
#include "QuicConnectionClient.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/lifecycle/LifecycleOperation.h"
#include "connection_config_data_m.h"




namespace inet {

class SimV1_client: public ApplicationBase { //public cSimpleModule {
public:
    SimV1_client();
    virtual ~SimV1_client();

    virtual void initialize(int stage) override;
    //virtual void handleMessage(cMessage *msg) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
    void fsmClient(cMessage *msg);
    Packet* createDataPacket(long sendBytes);
    void connect();
    void sendData();
    void close();




protected:
 //QuicConnection* my_connection=nullptr;
 cMessage *fsm_state;

 simtime_t tOpen;
 simtime_t tSend;
 simtime_t tClose;

 //long total_bytes_to_send;
 //int* streams;


 int current_connectionID;//for the future to save the current connection id.
 bool first_connection;//curentlly bool in the future need to compare connection id input with "current_connectionID" or to save all previous connections
};

Define_Module(SimV1_client);

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_SimV1_client_CLIENT_H_ */
