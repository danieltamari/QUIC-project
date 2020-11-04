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

#include <omnetpp.h>
#include "headers_and_frames/QuicFrame_m.h"
#include "headers_and_frames/StreamFrame_m.h"


namespace inet {

struct stream {
    int stream_id;
    int current_offset_in_stream; // stream send frame from this current offset
    bool valid;
    int stream_size; // how many bytes stream have to send
    int ACKed_bytes;
    bool stream_done;

    // flow control parameters
    int flow_control_recieve_window; // receive window size per stream
    int bytes_left_to_send_in_stream; // how many bytes we are left to send in this stream
};


class QuicStreamArr {
public:
    QuicStreamArr();
    QuicStreamArr(int streams_num);
    virtual ~QuicStreamArr();
    void addNewStream(int stream_size, int stream_id);
    void addNewStreamServer(int stream_id, int inital_stream_window);
    void closeStream(int stream_id);
    void blockStream(int stream_id);
    void freeStream(int stream_id);
    void updateACKedBytes(int stream_id, int num_of_bytes);
    bool getAllStreamsDone();
    std::vector<IntrusivePtr<StreamFrame>>* framesToSend(int max_payload);
    bool isStreamExist(int stream_id);
    void setAllStreamsWindows(int window_size);
    void updateStreamFlowWindow(int stream_id, int acked_data_size);
    int getStreamNumber();
    void setStreamNumber(int new_stream_number);
    int getSumStreamsWindowSize();
    int getStreamBytesSent(int stream_id);


private:
    std::vector<stream*> stream_arr_; // vector of the streams
    int number_of_streams;
    int last_stream_index_checked;

};


} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICSTREAMARR_H_ */
