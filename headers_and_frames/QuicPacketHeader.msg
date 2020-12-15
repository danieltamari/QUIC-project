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
namespace inet;


//
// TODO generated message class
//
class QuicPacketHeader extends FieldsChunk {
    unsigned int dest_connection_id_length; // length in bytes of the Destination Connection ID field that follows it. This length is encoded as an 8-bit unsigned integer
	int dest_connection_ID;
	B packet_number_length; // contain the length of the packet number, encoded as an unsigned, two-bit integer that is one less than the length of the packet number field in bytes
	int packet_number;
	b header_form;
	b fixed_bit; // The next bit of byte 0 is set to 1. Packets containing a zero value for this bit are not valid packets in this version and MUST be discarded.
}
