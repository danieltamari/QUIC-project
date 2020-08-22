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

#include "StreamsData.h"

namespace inet {


StreamsData::StreamsData() {
   // frame_arr = new stream_frame[arr_size];
    total_size_in_bytes = 0;
}

StreamsData::~StreamsData() {
    // TODO Auto-generated destructor stub
}
stream_frame* StreamsData::AddNewFrame(int stream_id,int offset, int length, bool is_FIN) {
//    if (index > this->arr_size_) {
//        return;
//        //("need to throw here some errorr!!!!!1");
//    }

    stream_frame* new_frame = new stream_frame;
    new_frame->stream_id = stream_id;
    new_frame->length = length;
    new_frame->offset = offset;
    new_frame->is_FIN = is_FIN;
    frame_arr.push_back(new_frame);
    total_size_in_bytes += length;
   // number_of_frames+=1;
    return new_frame;
}

int StreamsData::getStreamID(int index) const {
  //  return frame_arr[index].stream_id;
}

int StreamsData::getOffset(int index) const {
   // return frame_arr[index].offset;
}

int StreamsData::getLength(int index) const {
  //  return frame_arr[index].length;
}

bool StreamsData::getFIN(int index) const {
  //  return frame_arr[index].is_FIN;
}

int StreamsData::getTotalSize() const {
    return total_size_in_bytes;
}

int StreamsData::getNumFrames()const {
   // return number_of_frames;
}
} /* namespace inet */
