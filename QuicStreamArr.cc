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

#include "QuicStreamArr.h"
#include "IndexQueueNodes_m.h"
#include <iostream>

#define MAX_BYTES 300//will be a parameter in the future
# define STREAM_SIZE INT_MAX-1

namespace inet {

/*
 * we create a new empty stream array and insert all the cells to the free index queue (because we didn't open any stream yet.
 */
QuicStreamArr::QuicStreamArr(int streams_num) {
    stream_arr_ = new stream[streams_num];
    max_streams_num_ = streams_num;
    number_of_streams = streams_num;

    ///#####
    valid_streams_num_ = 0;
    curr_max_stream_ = 0;
    //####

    for (int index = 0; index < streams_num; index++) {
        this->AddNewStream(MAX_BYTES, index);
        IndexQueueNodes *queue_node;
        queue_node = new IndexQueueNodes;
        queue_node->setIndex(index);
        this->avilable_streams_queue_.insert(queue_node);
    }
}

void QuicStreamArr::AddNewStream(int max_bytes, int index) {
    this->stream_arr_[index].bytes_in_stream = 0;
    this->stream_arr_[index].max_bytes_to_send = max_bytes;
    this->stream_arr_[index].stream_id = index;
    this->stream_arr_[index].valid = true;
    this->stream_arr_[index].current_offset_in_stream = 0;
    this->stream_arr_[index].stream_size = STREAM_SIZE;
}

bool QuicStreamArr::CloseStream(int stream_id) {
    for (int i = 0; i < this->max_streams_num_; i++) {
        if (this->stream_arr_[i].stream_id == stream_id) {
            //IndexQueueNodes *queue_node;
            //queue_node = new IndexQueueNodes;
            //queue_node->setIndex(i);
            this->valid_streams_num_--;
            this->stream_arr_[i].valid = false;
            return true;
        }
    }
    return false;
}

bool QuicStreamArr::IsAvilableStreamExist() {
    if (this->avilable_streams_queue_.isEmpty()) {
        return false;
    }
    return true;
}

StreamsData* QuicStreamArr::DataToSend(int bytes_in_packet) {
    int checked_streams = 0;
    int index_in_frame_array = 0;
    bool packet_full = false;
    int bytes_left_to_send = bytes_in_packet;
    StreamsData* new_data = new StreamsData(this->number_of_streams);
    while (!packet_full && checked_streams != this->number_of_streams) {
        bool isFin = false;
        int bytes_to_send_in_frame;
        IndexQueueNodes *current_stream_node =
                (IndexQueueNodes*) avilable_streams_queue_.pop(); //find new available stream.
        int stream_id = current_stream_node->getIndex();
        checked_streams++;
        //int free_bytes_in_stream;
        //        this->stream_arr_[stream_id].max_bytes_to_send- this->stream_arr_[stream_id].bytes_in_stream;
        //if (free_bytes_in_stream == 0) {
        //    this->avilable_streams_queue_.insert(current_stream_node);
        //    continue;
        //}
        if (bytes_left_to_send > this->stream_arr_[stream_id].max_bytes_to_send)
            bytes_to_send_in_frame =
                    this->stream_arr_[stream_id].max_bytes_to_send;
        else
            bytes_to_send_in_frame = bytes_left_to_send;
        this->stream_arr_[stream_id].bytes_in_stream += bytes_to_send_in_frame;
        int offset = this->stream_arr_[stream_id].current_offset_in_stream;
        int stream_size = this->stream_arr_[stream_id].stream_size;
        if (bytes_to_send_in_frame + offset >= stream_size) {
            bytes_to_send_in_frame = stream_size - offset;
            isFin = true;
        }
        new_data->AddNewFrame(index_in_frame_array, stream_id,
                stream_arr_[stream_id].current_offset_in_stream,
                bytes_to_send_in_frame, isFin);
        this->stream_arr_[stream_id].current_offset_in_stream +=
                bytes_to_send_in_frame;
        bytes_left_to_send -= bytes_to_send_in_frame;

        this->avilable_streams_queue_.insert(current_stream_node);
        if (bytes_left_to_send == 0) {
            packet_full = true;
        }
        index_in_frame_array++;
    }
    return new_data;
}

QuicStreamArr::~QuicStreamArr() {
// TODO Auto-generated destructor stub
}

} /* namespace inet */
