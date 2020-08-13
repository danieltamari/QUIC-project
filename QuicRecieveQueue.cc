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

#include "QuicReceiveQueue.h"

namespace inet {

QuicRecieveQueue::QuicRecieveQueue() {
    // TODO Auto-generated constructor stub
    streams_ = new stream_information[NUM_OF_STREAMS];
    for (int i = 0; i < NUM_OF_STREAMS; i++) {
        this->streams_[i].final_size = -1 ;
        this->streams_[i].last_frame_received = false;
        this->streams_[i].buffer = new ReorderBuffer();
    }
}

/*
bool QuicRecieveQueue::isStreamIDExist(int stream_id) {
    // check if stream exist on the map
    try {
        stream_information* info = streams_.at(stream_id);
        delete info;
        return true;
    }
    catch(const std::out_of_range& oor) {
        return false;
    }
}
*/

void QuicRecieveQueue::updateBuffer(int stream_id, int offset, int length) {
    // make chunk of data according to the length field
    const auto& data = makeShared<FieldsChunk>();
    data->setChunkLength(B(length));
/*
    // if stream_id doesn't exist on the map, make a new entry
    if (!isStreamIDExist(stream_id)) {
        stream_information new_stream;
        new_stream.buffer = new ReorderBuffer();
        new_stream.last_frame_received = false;
        new_stream.final_size = -1;
        streams_.insert(std::pair<int, stream_information*>(stream_id, &new_stream));
    }
 */
    // update the reorder buffer according to the frame offset
    stream_information info = streams_[stream_id];
    info.buffer->replace(B(offset), data);
}

void QuicRecieveQueue::updateFinal(int stream_id, int offset, int length) {
    // enter this function if frame is  marked with isFIN flag
    /*
    if (!isStreamIDExist(stream_id)) {
        stream_information new_stream;
        new_stream.buffer = new ReorderBuffer();
        new_stream.last_frame_received = true;
        streams_.insert(std::pair<int, stream_information*>(stream_id, &new_stream));
    }
*/
    // update the final_size field as mentioned in RFC
    //stream_information info = streams_[stream_id];
    streams_[stream_id].final_size = offset + length;
}

bool QuicRecieveQueue::check_if_ended(int stream_id) {
    stream_information info = streams_[stream_id];
    if (info.final_size != -1 && info.buffer->getAvailableDataLength() == B(info.final_size)) { // stream finished
       // streams_.erase(stream_id); // remove the stream entry from the map
        return true;
    }
    return false;
}



QuicRecieveQueue::~QuicRecieveQueue() {
    // TODO Auto-generated destructor stub
}

} /* namespace inet */




