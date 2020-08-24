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

#include "QuicSendQueue.h"

namespace inet {

QuicSendQueue::QuicSendQueue() {
    // TODO Auto-generated constructor stub
    // send_queue = new ChunkQueue();
}


void QuicSendQueue::addStreamFrame(stream_frame* frame) {
    this->sent_without_ACK_frames.push_back(frame);
}

bool QuicSendQueue ::isQueueEmpty(){
//    B bytes_left=send_queue.getLength();
//    if (bytes_left==B(0))
//        return true;
//    else
//        return false;
}

QuicSendQueue::~QuicSendQueue() {
    // TODO Auto-generated destructor stub
}

} /* namespace inet */
