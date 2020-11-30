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

QuicReceiveQueue::QuicReceiveQueue() {
    // TODO Auto-generated constructor stub
    streams_status = new std::map<int,stream_receive_state*>();
}


QuicReceiveQueue::~QuicReceiveQueue() {
    // TODO Auto-generated destructor stub
}

void QuicReceiveQueue::updateStreamStatus(int stream_id,int offset,int length,bool is_FIN) {
  // check if stream exist in streams_status
    std::map<int,stream_receive_state*>::iterator it = streams_status->find(stream_id);
    if (it == streams_status->end()) {
        stream_receive_state* stream_info = new stream_receive_state;
        stream_info->final_size = -1;
        stream_info->last_frame_received = false;
        stream_info->num_bytes_received = length;
        streams_status->insert({stream_id,stream_info});
    }
    else {
        stream_receive_state* curr_stream_info = streams_status->at(stream_id);
        curr_stream_info->num_bytes_received += length;
    }

   if (is_FIN) {
       // update the final_size field as mentioned in RFC
       stream_receive_state* curr_stream_info = streams_status->at(stream_id);
       curr_stream_info->final_size = offset + length;
       curr_stream_info->last_frame_received = true;
   }
}


bool QuicReceiveQueue::checkIfEnded(int stream_id) {
    stream_receive_state* curr_stream_info = streams_status->at(stream_id);
    if (curr_stream_info->last_frame_received && curr_stream_info->final_size == curr_stream_info->num_bytes_received)
        return true;
    else
        return false;
}


void QuicReceiveQueue::removeStreamStatus(int stream_id) {
    streams_status->erase(stream_id);
}



} /* namespace inet */




