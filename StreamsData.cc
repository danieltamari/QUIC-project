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

StreamsData::StreamsData(){
    frame_arr=NULL;
}

StreamsData::StreamsData(int arr_size) {
    // TODO Auto-generated constructor stub
    arr_size_ = arr_size;
    frame_arr = new stream_frame[arr_size];
    total_size_in_bytes = 0;
    this->number_of_frames=0;
}

StreamsData::~StreamsData() {
    // TODO Auto-generated destructor stub
}
void StreamsData::AddNewFrame(int index, int stream_id,int offset, int length, bool is_FIN) {
    if (index > this->arr_size_) {
        return;
        //("need to throw here some errorr!!!!!1");
    }
    this->frame_arr[index].length=length;
    this->frame_arr[index].offset=offset;
    this->frame_arr[index].stream_id=stream_id;
    this->frame_arr[index].is_FIN=is_FIN;
    total_size_in_bytes += length;
    number_of_frames+=1;
}

int StreamsData::getStreamID(int index) const {
    return stream_arr[index].stream_id;
}

int StreamsData::getOffset(int index) const {
    return stream_arr[index].offset;
}

int StreamsData::getLength(int index) const {
    return stream_arr[index].length;
}

bool StreamsData::getFIN(int index) const {
    return stream_arr[index].is_FIN;
}

int StreamsData::getTotalSize() const {
    return total_size_in_bytes;
}

int StreamsData::getNumFrames()const {
    return number_of_frames;
}
} /* namespace inet */
