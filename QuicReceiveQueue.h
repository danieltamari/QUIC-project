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


#include "inet/common/packet/ReorderBuffer.h"
#include "inet/common/packet/chunk/FieldsChunk.h"
#include "StreamsData.h"
#include <map>

#ifndef INET_APPLICATIONS_QUICAPP_QUICRECEIVEQUEUE_H_
#define INET_APPLICATIONS_QUICAPP_QUICRECEIVEQUEUE_H_
#define NUM_OF_STREAMS 10 // will be parameters later

namespace inet {

struct stream_information {
    int final_size; // update after the last frame in received
    bool last_frame_received; // true if the final frame of this stream (marked with isFIN) is received
    ReorderBuffer* buffer; // buffer to restore all the incoming data from frames OOO
};

class QuicRecieveQueue {
public:
    QuicRecieveQueue();
    virtual ~QuicRecieveQueue();
    void updateBuffer(int stream_id, int offset, int length);
    bool isStreamIDExist(int stream_id);
    void updateFinal(int stream_id, int offset, int length);
    bool check_if_ended(int stream_id);

protected:
    //std::map<int, stream_information*> streams_; // map between key -> stream id to it's information
    stream_information* streams_; // array of the streams info
};

} /* namespace inet */



#endif /* INET_APPLICATIONS_QUICAPP_QUICRECEIVEQUEUE_H_ */
