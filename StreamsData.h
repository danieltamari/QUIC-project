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

#include "inet/common/INETDefs.h"
#include "inet/common/packet/chunk/Chunk.h"

#ifndef INET_APPLICATIONS_QUICAPP_STREAMSDATA_H_
#define INET_APPLICATIONS_QUICAPP_STREAMSDATA_H_

namespace inet {

struct stream_frame {
    int stream_id;
    int offset;
    int length;
    bool is_FIN;
};

/*
 * This class represent the data that we transfer inside the packet.
 * in each packet there are number of stream frames.
 */
class StreamsData {
public:
    StreamsData();
    stream_frame* AddNewFrame(int stream_id,int offset, int length, bool is_FIN);
    std::vector<stream_frame*> getFramesArray() const;
    int getTotalSize() const;
    virtual ~StreamsData();

private:
   // int arr_size_;
    std::vector<stream_frame*> frame_arr;
    int total_size_in_bytes;
   // int number_of_frames;
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_STREAMSDATA_H_ */
