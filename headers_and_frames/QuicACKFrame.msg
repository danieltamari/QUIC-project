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

import inet.common.INETDefs;
import inet.applications.base.ApplicationPacket;
import inet.common.packet.chunk.Chunk;


cplusplus {{
#include "QuicConnection.h"

}}

//struct range;

struct range {
    int gap;
    int ACK_range_length;
};

namespace inet;


//
// TODO generated message class
//
class QuicACKFrame extends FieldsChunk {
	int largest_acknowledged; //  largest packet number the peer is acknowledging- the largest packet number that the peer has received prior to generating the ACK frame.
	int ACK_delay; // acknowledgement delay in microseconds
	int ACK_range_count;
	int first_ACK_range; // number of contiguous packets preceding the Largest Acknowledged that are being acknowledged
	range ACK_ranges[];
}
