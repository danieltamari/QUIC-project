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
import inet.applications.quicapp.headers_and_frames.QuicPacketHeader;
namespace inet;


//
// TODO generated message class
//
class QuicShortHeader extends QuicPacketHeader {// ; FieldsChunk {
    // Packets with the short header are used after a connection is established and 1-RTT keys are available (RFC 12)
    
    b header_form; // The most significant bit of byte 0 is set to 1 for long headers
    b fixed_bit; // The next bit of byte 0 is set to 1. Packets containing a zero value for this bit are not valid packets in this version and MUST be discarded.
	b spin_bit;
	B reserved_bits;
	B packet_number_length; // contain the length of the packet number, encoded as an unsigned, two-bit integer that is one less than the length of the packet number field in bytes
//	int dest_connection_ID; // The Destination Connection ID is a connection ID that is chosen by the intended recipient of the packet, max size is 20 Bytes
//	int packet_number; //  The packet number field is 1 to 4 bytes long
	// remove later
	int packet_type;
	
	// total size(in bytes): 2 + packet_number_length + dest_connection_id_length
}
