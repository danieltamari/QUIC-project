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


void QuicSendQueue::addAllData(int total_bytes_to_send) {
    const auto& data_to_send = makeShared<FieldsChunk>();
    data_to_send->setChunkLength(B(total_bytes_to_send));
    send_queue.push(data_to_send);
}

void QuicSendQueue::removeDataSent(int msgByteLength) {
   const Ptr<const Chunk> sent_data = send_queue.pop(B(msgByteLength));
}

QuicSendQueue::~QuicSendQueue() {
    // TODO Auto-generated destructor stub
}

} /* namespace inet */
