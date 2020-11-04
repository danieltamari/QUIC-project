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


#ifndef INET_APPLICATIONS_QUICAPP_QUICRECEIVEQUEUE_H_
#define INET_APPLICATIONS_QUICAPP_QUICRECEIVEQUEUE_H_


namespace inet {

struct stream_information {
    int final_size; // update after the last frame in received
    bool last_frame_received; // true if the final frame of this stream (marked with isFIN) is received
    int num_bytes_received;
};


class QuicReceiveQueue {
public:
    QuicReceiveQueue();
    virtual ~QuicReceiveQueue();
    bool checkIfEnded(int stream_id);
    void updateStreamInfo(int stream_id,int offset,int length,bool is_FIN);
    void removeStreamInfo(int stream_id);


protected:
    std::map<int,stream_information*>* streams_info;


};

} /* namespace inet */



#endif /* INET_APPLICATIONS_QUICAPP_QUICRECEIVEQUEUE_H_ */
