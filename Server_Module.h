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

#ifndef INET_APPLICATIONS_QUICAPP_SERVER_MODULE_H_
#define INET_APPLICATIONS_QUICAPP_SERVER_MODULE_H_

#include "inet/common/INETDefs.h"
#include <omnetpp.h>
#include "QuicConnectionServer.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/lifecycle/LifecycleOperation.h"

namespace inet {

class Server_Module : public ApplicationBase {
public:
    Server_Module();
    virtual ~Server_Module();
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
    void fsmServer(cMessage *msg);
    void connect();
    void close();


protected:
    cMessage *fsm_state;

    simtime_t tOpen;
    simtime_t tClose;

};
Define_Module(Server_Module);

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_SERVER_MODULE_H_ */
