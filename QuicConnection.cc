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

#include "QuicConnection.h"

namespace inet {


QuicConnection::QuicConnection() {
    char fsmname[24];
    sprintf(fsmname, "quic_fsm");
    fsm.setName(fsmname);
    this->event = new cMessage("INITAL");
}


QuicConnection::~QuicConnection() {
    // TODO Auto-generated destructor stub
}


int QuicConnection::GetSourceID() {
    return connection_source_ID;
}


void QuicConnection::SetSourceID(int source_ID) {
    connection_source_ID = source_ID;
}


int QuicConnection::GetDestID() {
    return connection_dest_ID;
}


void QuicConnection::SetDestID(int dest_ID) {
    connection_dest_ID = dest_ID;
}


L3Address QuicConnection::GetDestAddress(){
    return destination;
}


unsigned int QuicConnection::calcSizeInBytes(int number) {
    int i = 0;
    int counter = 0;
    if (number == 0)
        return 1;
    while (number > 0) {
        number = number / 2;
        i++;
    }
    for (int j = i - 1; j >= 0; j--)
        counter++;
    return std::ceil(counter/8.0); // return in bytes
}


int QuicConnection::calcTotalSize(std::vector<IntrusivePtr<StreamFrame>>* frames_to_send) {
    int total_bytes = 0;
    for (std::vector<IntrusivePtr<StreamFrame>>::iterator it =
            frames_to_send->begin(); it != frames_to_send->end(); ++it) {
        total_bytes += (*it)->getLength();
    }
    return total_bytes;
}


int QuicConnection::calcHeaderSize(bool short_header) {
  //  unsigned int packet_number_len = calcSizeInBytes(packet_counter);
    int size = 0;
    unsigned int packet_number_len = 2;
    if (short_header) {
        unsigned int dest_ID_len = calcSizeInBytes(connection_dest_ID);
        size = SHORT_HEADER_BASE_LENGTH + dest_ID_len + packet_number_len;
    }
    else { // long header
        unsigned int dest_ID_len = calcSizeInBytes(connection_dest_ID);
        unsigned int src_ID_len = calcSizeInBytes(connection_source_ID);
        size = LONG_HEADER_BASE_LENGTH + dest_ID_len + src_ID_len + packet_number_len;
    }
    return size;
}

} /* namespace inet */
