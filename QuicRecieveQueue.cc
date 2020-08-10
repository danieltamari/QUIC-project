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
}


bool QuicRecieveQueue::isStreamIDExist(int stream_id) {
    try {
        stream_information* info = streams_.at(stream_id);
        return true;
    }
    catch(const std::out_of_range& oor) {
        return false;
    }
}

void QuicRecieveQueue::updateBuffer(int stream_id, int offset, int length) {
    const auto& data = makeShared<FieldsChunk>();
    data->setChunkLength(B(length));

    if (!isStreamIDExist(stream_id)) {
        stream_information new_stream;
        new_stream.buffer = new ReorderBuffer();
        new_stream.last_frame_sent = false;
        new_stream.final_size = -1;
        streams_.insert(std::pair<int, stream_information*>(stream_id, &new_stream));
    }

    stream_information* info = streams_.at(stream_id);
    info->buffer->replace(B(offset), data);
}

void QuicRecieveQueue::updateFinal(int stream_id, int offset, int length) {
    if (!isStreamIDExist(stream_id)) {
        stream_information new_stream;
        new_stream.buffer = new ReorderBuffer();
        new_stream.last_frame_sent = true;
        streams_.insert(std::pair<int, stream_information*>(stream_id, &new_stream));
    }

    stream_information* info = streams_.at(stream_id);
    info->final_size = offset + length;
}

bool QuicRecieveQueue::check_if_ended(int stream_id) {
    stream_information* info = streams_.at(stream_id);
    if (info->final_size != -1 && info->buffer->getAvailableDataLength() == B(info->final_size)) { // stream finished
        streams_.erase(stream_id);
        return true;
    }
    return false;
}



QuicRecieveQueue::~QuicRecieveQueue() {
    // TODO Auto-generated destructor stub
}

} /* namespace inet */




