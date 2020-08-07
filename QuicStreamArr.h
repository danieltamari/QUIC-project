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

#ifndef INET_APPLICATIONS_QUICAPP_QUICSTREAMARR_H_
#define INET_APPLICATIONS_QUICAPP_QUICSTREAMARR_H_

#include "StreamsData.h"
#include <omnetpp.h>

/*
 * this class represent the stream data array in the connection.
 * each time a packet with data is sent or recieved each side of the connection
 * should update his stream data array accordingly.
 */

namespace inet {

struct stream {
    int stream_id;
    int max_bytes_to_send; // total data size that stream can send simultansiolsley
    int bytes_in_stream; // total bytes currently in this stream
    int current_offset_in_stream;
    bool valid;
    int stream_size; //


//    int sent_bytes_num; // the number of bytes that was sent/recived in this stream
//    bool valid; // is the stream open or close, if close we can open a new stream in this cell with new stream_id
//    int max_bytes;// the maximum number of bytes that can be sent in this stream;
//    int curr_quanta; // the quanta we can send through this stream
//    int total_bytes_to_send; // total data size we want to transfer in this stream
//    int bytes_left_to_send; // how many bytes we are left to send in this stream
};

class QuicStreamArr {
public:
    QuicStreamArr(int streams_num);
    virtual ~QuicStreamArr();
    int AddNewStream(int max_bytes, int index);
    bool CloseStream(int stream_id);
    bool IsAvilableStreamExist();
    int FreeBytesAvilable() {return total_free_bytes_;}
    StreamsData* DataToSend(int frames_number, int total_bytes_to_send);//make a streamData to send


    // void UpdateStremMaxBytes(int stream_id, int max_bytes); ####->connects to flow_control rfc 27 page 21 , in the future.


private:
    stream* stream_arr_; // array of the streams
    int max_streams_num_; // the maximum number of streams that can send simultaneously .
    int valid_streams_num_; // current number of open streams.
    int curr_max_stream_;   // the stream with biggest stream_id
    //omnetpp::cQueue free_index_queue_; //this queue holds the indices of the free cells in the stream array. so we can write the data to those cells.
    omnetpp::cQueue avilable_streams_queue_;//this queue holds the indices in the array of the streams that are not full and we can send data through them.
    int total_free_bytes_;

    int number_of_streams;

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICSTREAMARR_H_ */
