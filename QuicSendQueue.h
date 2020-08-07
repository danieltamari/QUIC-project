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

#include "inet/common/packet/ChunkQueue.h"
#include "inet/common/packet/chunk/FieldsChunk.h"

#ifndef INET_APPLICATIONS_QUICAPP_QUICSENDQUEUE_H_
#define INET_APPLICATIONS_QUICAPP_QUICSENDQUEUE_H_


namespace inet {

class QuicSendQueue {
public:
    QuicSendQueue();
    virtual ~QuicSendQueue();
    void addAllData(int total_bytes_to_send);
    void removeDataSent(int msgByteLength);

protected:
    ChunkQueue send_queue; // stores application data
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICSENDQUEUE_H_ */
