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

QuicRecieveQueue::QuicRecieveQueue(int stream_id) {
    stream_info = new stream_information;
    stream_info->stream_id = stream_id;
    stream_info->final_size = -1 ;
    stream_info->last_frame_received = false;
    stream_info->buffer = new ReorderBuffer();
}

void QuicRecieveQueue::addStreamFrame(stream_frame* frame) {
    this->received_frames.push_back(frame);
}

void QuicRecieveQueue::updateStreamInfo(int offset,int length,bool is_FIN) {
    // make chunk of data according to the length field
    const auto& data = makeShared<FieldsChunk>();
    data->setChunkLength(B(length));

   // update the reorder buffer according to the frame offset
   stream_info->buffer->replace(B(offset), data);

   if (is_FIN) {
       // update the final_size field as mentioned in RFC
       stream_info->final_size = offset + length;
       stream_info->last_frame_received = true;
   }
}

int QuicRecieveQueue::getStreamID() {
    return this->stream_info->stream_id;
}


int QuicRecieveQueue::getfinal_size() {
   return this->stream_info->final_size;
}


bool QuicRecieveQueue::getlast_frame_received() {
    return this->stream_info->last_frame_received;
}

bool QuicRecieveQueue::check_if_ended() {
    if (this->stream_info->final_size != -1 && this->stream_info->buffer->getAvailableDataLength() ==
            B(this->stream_info->final_size)) { // stream finished
        return true;
    }
    return false;
}



QuicRecieveQueue::~QuicRecieveQueue() {
    // TODO Auto-generated destructor stub
}

} /* namespace inet */




