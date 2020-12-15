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
	b spin_bit;
	B reserved_bits;	
	
	// total size(in bytes): 2 + packet_number_length + dest_connection_id_length
}
