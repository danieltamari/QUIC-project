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

#ifndef INET_APPLICATIONS_QUICAPP_SIMV1SERVER_H_
#define INET_APPLICATIONS_QUICAPP_SIMV1SERVER_H_

#include "inet/common/INETDefs.h"
#include <omnetpp.h>
#include "QuicConnection.h"

namespace inet {

class SimV1_server : public cSimpleModule {
public:
    SimV1_server();
    virtual ~SimV1_server();

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

protected:
    QuicConnection* my_connection = nullptr;
    cMessage *start_fsm;
};
Define_Module(SimV1_server);

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_SIMV1SERVER_H_ */
