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

QuicStreamArr::QuicStreamArr() {
    number_of_streams = 0;
    last_stream_index_checked = 0;
}


QuicStreamArr::QuicStreamArr(int streams_num) {
    number_of_streams = streams_num;
    last_stream_index_checked=0;
}


QuicStreamArr::~QuicStreamArr() {
// TODO Auto-generated destructor stub
}


void QuicStreamArr::addNewStream(int stream_size, int stream_id) {
    stream* stream_to_add = new stream;
    stream_to_add->stream_id = stream_id;
    stream_to_add->valid = true;
    stream_to_add->current_offset_in_stream = 0;
    stream_to_add->stream_size = stream_size;
    stream_to_add->flow_control_recieve_window = 0;
    stream_to_add->bytes_left_to_send_in_stream = stream_size;
    stream_to_add->ACKed_bytes = 0;
    stream_to_add->stream_done = false;
    stream_arr_.push_back(stream_to_add);
}


void QuicStreamArr::addNewStreamServer(int stream_id, int inital_stream_window) {
    stream* stream_to_add=new stream;
    stream_to_add->stream_id = stream_id;
    stream_to_add->valid = true;
    stream_to_add->current_offset_in_stream = 0;
    stream_to_add->flow_control_recieve_window = inital_stream_window;
    stream_to_add->ACKed_bytes = 0;
    stream_to_add->stream_done = false;
    stream_arr_.push_back(stream_to_add);
    number_of_streams++;
}


void QuicStreamArr::closeStream(int stream_id) {
    for (std::vector<stream*>::iterator it =
            stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if (stream_id == (*it)->stream_id) {
            (*it)->stream_done = true;
            break;
        }
    }
}


void QuicStreamArr::blockStream(int stream_id) {
    for (std::vector<stream*>::iterator it =
            stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if (stream_id == (*it)->stream_id) {
            (*it)->valid = false;
            break;
        }
    }
}


void QuicStreamArr::freeStream(int stream_id) {
    for (std::vector<stream*>::iterator it =
            stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if (stream_id == (*it)->stream_id) {
            (*it)->valid = true;
            break;
        }
    }
}


void QuicStreamArr::updateACKedBytes(int stream_id, int num_of_bytes) {
    for (std::vector<stream*>::iterator it =
            stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if (stream_id == (*it)->stream_id) {
            (*it)->ACKed_bytes += num_of_bytes;
            EV << "############## ACKed_bytes of stream " << (*it)->stream_id << " is: " <<  (*it)->ACKed_bytes << " ##############" << endl;

            if ((*it)->ACKed_bytes == (*it)->stream_size) {
                // stream is done
                EV << "****** stream " << (*it)->stream_id << " ended at client" << endl;
                closeStream(stream_id);
            }
            break;
        }
    }
}


bool QuicStreamArr::getAllStreamsDone() {
    int number_of_done_streams = 0;
    for (std::vector<stream*>::iterator it =
            stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if((*it)->stream_done)
            number_of_done_streams++;
    }
    if (number_of_done_streams == number_of_streams)
        return true;
    return false;
}


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

    EV << "##########################" << endl;
    EV << " *********** current packet content: ***********" << endl;
    // fill the given payload with data from streams
    while (!packet_full && checked_streams != number_of_streams) {
        bool isFin = false;
        int bytes_to_send_in_frame;
        stream* curr_stream = stream_arr_[last_stream_index_checked];
        checked_streams++;
        if(!curr_stream->valid){ // stream is blocked -> move to the next one
            last_stream_index_checked++;
            if (last_stream_index_checked == number_of_streams) // finished to go over all the streams
                last_stream_index_checked = 0;
            continue;
        }
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
        new_frame->setFrame_type(5);
        new_frame->setChunkLength(B(bytes_to_send_in_frame));
        frames_vector->push_back(new_frame);

        EV << "*** stream_id is " << stream_id << " offset is " << offset << " length is " << bytes_to_send_in_frame << endl;
        curr_stream->current_offset_in_stream += bytes_to_send_in_frame;
        bytes_left_to_send_in_packet -= bytes_to_send_in_frame;

        if (bytes_left_to_send_in_packet == 0) {
            packet_full = true;
        }

        last_stream_index_checked++;
        if (last_stream_index_checked == number_of_streams)
            last_stream_index_checked = 0;
    }

    return frames_vector;
}


bool QuicStreamArr::isStreamExist(int stream_id) {
    for (std::vector<stream*>::iterator it =
                 stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if ((*it)->stream_id == stream_id)
            return true;
    }
    return false;
}


void QuicStreamArr::setAllStreamsWindows(int window_size) {
    for (std::vector<stream*>::iterator it =
                 stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        (*it)->flow_control_recieve_window = window_size - ((*it)->stream_size - (*it)->bytes_left_to_send_in_stream);
    }
}


void QuicStreamArr::updateStreamFlowWindow(int stream_id, int acked_data_size) {
    for (std::vector<stream*>::iterator it =
                 stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if ((*it)->stream_id == stream_id) {
            (*it)->flow_control_recieve_window += acked_data_size;
            break;
        }
    }
}


int QuicStreamArr::getStreamNumber(){
    return number_of_streams;
}


void QuicStreamArr::setStreamNumber(int new_stream_number){
    number_of_streams=new_stream_number;
}


int QuicStreamArr::getSumStreamsWindowSize() {
    int sum = 0;
    for (std::vector<stream*>::iterator it =
                 stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        sum += (*it)->flow_control_recieve_window;
    }
    return sum;
}


int QuicStreamArr::getStreamBytesSent(int stream_id) {
    for (std::vector<stream*>::iterator it =
                 stream_arr_.begin(); it != stream_arr_.end(); ++it) {
        if ((*it)->stream_id == stream_id)
            return (*it)->current_offset_in_stream;
    }
}


} /* namespace inet */
