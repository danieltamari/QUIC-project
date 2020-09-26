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
#include <iostream>


namespace inet {

/*
 * we create a new empty stream array and insert all the cells to the free index queue (because we didn't open any stream yet.
 */

QuicStreamArr::QuicStreamArr() {
    max_streams_num_ = 0;
    number_of_streams = 0;
    last_stream_index_checked=0;
  //  stream_arr_ = new std::vector<stream*>();
}


QuicStreamArr::QuicStreamArr(int streams_num) {
    //stream_arr_ = new stream[streams_num];
    max_streams_num_ = streams_num;
    number_of_streams = streams_num;
    last_stream_index_checked=0;
  //  stream_arr_ = new std::vector<stream*>();

    ///#####
    valid_streams_num_ = 0;
    curr_max_stream_ = 0;
    //####
}

void QuicStreamArr::AddNewStream(int stream_size, int stream_id) {
    stream* stream_to_add=new stream;
    stream_to_add->stream_id = stream_id;
    stream_to_add->valid = true;
    stream_to_add->current_offset_in_stream = 0;
    stream_to_add->stream_size = stream_size;
    stream_to_add->max_flow_control_window_size = 0;
    stream_to_add->flow_control_recieve_window = 0;
    stream_to_add->bytes_left_to_send_in_stream=stream_size;
    stream_to_add->send_queue = new QuicSendQueue();
    this->stream_arr_.push_back(stream_to_add);
}

void QuicStreamArr::AddNewStreamServer(int stream_id, int inital_stream_window) {
   // this->stream_arr_[index].bytes_in_stream = 0;
   // this->stream_arr_[index].max_bytes_to_send = max_bytes;
    stream* stream_to_add=new stream;
    stream_to_add->stream_id = stream_id;
    stream_to_add->valid = true;
    stream_to_add->current_offset_in_stream = 0;
    stream_to_add->max_flow_control_window_size = inital_stream_window;
  //  stream_to_add->highest_recieved_byte_offset = 0;
  //  stream_to_add->flow_control_recieve_offset = inital_stream_window;
    stream_to_add->flow_control_recieve_window = inital_stream_window;
  //  stream_to_add->consumed_bytes = 0;

   // stream_to_add->bytes_left_to_send_in_stream=stream_size;
    stream_to_add->receive_queue = new QuicRecieveQueue(stream_id);
    this->stream_arr_.push_back(stream_to_add);


}

bool QuicStreamArr::CloseStream(int stream_id) {
    for (int i = 0; i < this->max_streams_num_; i++) {
        if (this->stream_arr_[i]->stream_id == stream_id) {
            //IndexQueueNodes *queue_node;
            //queue_node = new IndexQueueNodes;
            //queue_node->setIndex(i);
            this->valid_streams_num_--;
            stream_arr_[i]->valid = false;
            return true;
        }
    }
    return false;
}

bool QuicStreamArr::IsAvilableStreamExist() {
//    if (this->avilable_streams_queue_.isEmpty()) {
//        return false;
//    }
//    return true;
}

void QuicStreamArr::blockStream(int stream_id) {
    for (std::vector<stream*>::iterator it =
            this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if (stream_id == (*it)->stream_id) {
            (*it)->valid = false;
            break;
        }
    }
    EV << "############## blocked stream: " << stream_id << " ##############" << endl;
}

void QuicStreamArr::freeStream(int stream_id) {
    for (std::vector<stream*>::iterator it =
            this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if (stream_id == (*it)->stream_id) {
            (*it)->valid = true;
            break;
        }
    }
    EV << "############## free stream: " << stream_id << " ##############" << endl;
}


//StreamsData* QuicStreamArr::DataToSend(int max_payload) {
//    StreamsData* new_data = new StreamsData();
//    int counter = 0;
//    for (std::vector<stream*>::iterator it =
//            this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
//        if (!(*it)->valid) {
//            counter++;
//        }
//    }
//
//    if (counter == number_of_streams)
//        return NULL;
//
//    int checked_streams = 0;
//  //  int connection_window_size = connection_flow_control_recieve_window;
//    bool packet_full = false;
//    int bytes_left_to_send_in_packet = max_payload;
//
//    while (!packet_full && checked_streams != this->number_of_streams) {
//        bool isFin = false;
//        int bytes_to_send_in_frame;
//
//        stream* curr_stream = stream_arr_[last_stream_index_checked];
//        checked_streams++;
//
//        if(!curr_stream->valid) // stream is blocked -> move to the next one
//            continue;
//
//        // check stream window (flow control)
//        if (bytes_left_to_send_in_packet > curr_stream->flow_control_recieve_window)
//            bytes_to_send_in_frame = curr_stream->flow_control_recieve_window;
//        else
//            bytes_to_send_in_frame = bytes_left_to_send_in_packet;
//
////        // check connection window
////        if (bytes_to_send_in_frame > connection_window_size)
////            bytes_to_send_in_frame = connection_window_size;
//
//        if (curr_stream->bytes_left_to_send_in_stream < bytes_to_send_in_frame)
//            bytes_to_send_in_frame = curr_stream->bytes_left_to_send_in_stream;
//
//        if (bytes_to_send_in_frame == 0) {//send BLOCKING FRAME IN FUTURE(??) ASK MICHA
//            last_stream_index_checked++;
//            if (last_stream_index_checked == number_of_streams)
//                last_stream_index_checked = 0;
//            continue;
//        }
////        if (connection_window_size==0){//end DATA_BLOCKING FRAME IN FUTURE(??) ASK MICHA
////            break;
////        }
//
//        // update stream flow control
//        curr_stream->bytes_left_to_send_in_stream -= bytes_to_send_in_frame;
////        curr_stream->highest_recieved_byte_offset+=bytes_to_send_in_frame;
//        curr_stream->flow_control_recieve_window -= bytes_to_send_in_frame;
//
//       // connection_window_size -= bytes_to_send_in_frame;
//
//        int stream_id = curr_stream->stream_id;
//        int offset = curr_stream->current_offset_in_stream;
//        int stream_size = curr_stream->stream_size;
//
//        if (bytes_to_send_in_frame + offset == stream_size) {
//            isFin = true;
//        }
//
//        stream_frame* new_frame = new_data->AddNewFrame(stream_id, offset, bytes_to_send_in_frame, isFin);
//        curr_stream->current_offset_in_stream += bytes_to_send_in_frame;
//        bytes_left_to_send_in_packet -= bytes_to_send_in_frame;
//
//      //  this->avilable_streams_queue_.insert(current_stream_node);
//        if (bytes_left_to_send_in_packet == 0) {
//            packet_full = true;
//        }
//
//        curr_stream->send_queue->addStreamFrame(new_frame);
//
//        last_stream_index_checked++;
//        if (last_stream_index_checked == number_of_streams)
//            last_stream_index_checked = 0;
//
//    }
//    // there is nothing to send (all streams are full ?)
//    if (bytes_left_to_send_in_packet == max_payload)
//        return NULL;
//
//    return new_data;
//}


std::vector<IntrusivePtr<StreamFrame>>* QuicStreamArr::framesToSend(int max_payload) {
    std::vector<IntrusivePtr<StreamFrame>>* frames_vector = new std::vector<IntrusivePtr<StreamFrame>>();
    int counter = 0;
    // if all streams are blocked -> return empty vector
    for (std::vector<stream*>::iterator it = stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if (!(*it)->valid) {
            counter++;
        }
    }
    if (counter == number_of_streams)
        return frames_vector;

    int checked_streams = 0;
    bool packet_full = false;
    int bytes_left_to_send_in_packet = max_payload;

    // fill the given payload with data from streams
    while (!packet_full && checked_streams != this->number_of_streams) {
        bool isFin = false;
        int bytes_to_send_in_frame;
        stream* curr_stream = stream_arr_[last_stream_index_checked];
        checked_streams++;
        if(!curr_stream->valid) // stream is blocked -> move to the next one
            continue;

        // check stream window (flow control)
        if (bytes_left_to_send_in_packet > curr_stream->flow_control_recieve_window)
            bytes_to_send_in_frame = curr_stream->flow_control_recieve_window;
        else
            bytes_to_send_in_frame = bytes_left_to_send_in_packet;

        // check if there are less bytes left to send on this stream than the total bytes given
        if (curr_stream->bytes_left_to_send_in_stream < bytes_to_send_in_frame)
            bytes_to_send_in_frame = curr_stream->bytes_left_to_send_in_stream;


        // ##### TO DO: check if client should send streamBlocked / dataBlocked ?

        if (bytes_to_send_in_frame == 0) {
            last_stream_index_checked++;
            if (last_stream_index_checked == number_of_streams) // finished to go over all the streams
                last_stream_index_checked = 0;
            continue;
        }

        // update stream flow control
        curr_stream->bytes_left_to_send_in_stream -= bytes_to_send_in_frame;
        curr_stream->flow_control_recieve_window -= bytes_to_send_in_frame;
        curr_stream->current_offset_in_stream += bytes_to_send_in_frame;
        bytes_left_to_send_in_packet -= bytes_to_send_in_frame;

        if (bytes_left_to_send_in_packet == 0) {
            packet_full = true;
        }
        last_stream_index_checked++;
        if (last_stream_index_checked == number_of_streams)
            last_stream_index_checked = 0;

        // create new stream frame
        int stream_id = curr_stream->stream_id;
        int offset = curr_stream->current_offset_in_stream;
        int stream_size = curr_stream->stream_size;

        if (bytes_to_send_in_frame + offset == stream_size) {
            isFin = true;
        }

        const auto &new_frame = makeShared<StreamFrame>();
        new_frame->setStream_id(stream_id);
        new_frame->setOffset(offset);
        new_frame->setLength(bytes_to_send_in_frame);
        new_frame->setIs_FIN(isFin);
        new_frame->setFrame_type(8);
        new_frame->setChunkLength(B(bytes_to_send_in_frame));
        frames_vector->push_back(new_frame);


       // curr_stream->send_queue->addStreamFrame(new_frame);
    }

    // there is nothing to send (all streams are full ?)
   // if (bytes_left_to_send_in_packet == max_payload)
   //     return NULL;

    return frames_vector;
}

void QuicStreamArr::setAllStreamsWindows(int window_size) {
    for (std::vector<stream*>::iterator it =
                    this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        (*it)->max_flow_control_window_size = window_size;
      //  (*it)->flow_control_recieve_offset = window_size;
        (*it)->flow_control_recieve_window = window_size;
    }
}


void QuicStreamArr::updateStreamFlowWindow(int stream_id, int acked_data_size) {
    for (std::vector<stream*>::iterator it =
                    this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if ((*it)->stream_id == stream_id) {
            (*it)->flow_control_recieve_window += acked_data_size;
            break;
        }
    }
}

int QuicStreamArr::getSumStreamsWindowSize() {
    int sum = 0;
    for (std::vector<stream*>::iterator it =
                    this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        sum += (*it)->flow_control_recieve_window;
    }
    return sum;
}

bool QuicStreamArr::isStreamExist(int stream_id) {
    for (std::vector<stream*>::iterator it =
                    this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if ((*it)->stream_id == stream_id)
            return true;
    }
    return false;
}

stream* QuicStreamArr::getStream(int stream_id) {
    for (std::vector<stream*>::iterator it =
                    this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if ((*it)->stream_id == stream_id)
            return *it;
    }
}

//int QuicStreamArr::getTotalConsumedBytes(){
//    int total_consumed_bytes=0;
//    for (std::vector<stream*>::iterator it =
//                    this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
//        total_consumed_bytes+=(*it)->consumed_bytes;
//    }
//    return total_consumed_bytes;
//}
//
//int QuicStreamArr::getTotalMaxOffset(){
//    int max_offset=0;
//    for (std::vector<stream*>::iterator it =
//                    this->stream_arr_.begin(); it != stream_arr_.end(); ++it) {
//        max_offset+=(*it)->highest_recieved_byte_offset;
//    }
//    return max_offset;
//}


QuicStreamArr::~QuicStreamArr() {
// TODO Auto-generated destructor stub
}

} /* namespace inet */
