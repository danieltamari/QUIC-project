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
class QuicLongHeader extends QuicPacketHeader {
    // Packets with the long header are Initial, 0-RTT, Handshake and Retry packets (RFC 12)
    
    b header_form; // The most significant bit of byte 0 is set to 1 for long headers
    b fixed_bit; // The next bit of byte 0 is set to 1. Packets containing a zero value for this bit are not valid packets in this version and MUST be discarded.
	int long_packet_type; // The next two bits of byte 0 contain a packet type:
//		0x0	Initial	
//		0x1	0-RTT	
//		0x2	Handshake	
//		0x3	Retry	
	int version; //32-bit field that follows the first byte. This field indicates the version of QUIC that is in use
	unsigned int dest_connection_id_length; // length in bytes of the Destination Connection ID field that follows it. This length is encoded as an 8-bit unsigned integer
	//int dest_connection_ID; // max size is 20 Bytes
	unsigned int source_connection_id_length; // length in bytes of the Destination Connection ID field that follows it. This length is encoded as an 8-bit unsigned integer
	int source_connection_ID; // max size is 20 Bytes
	
	// total size(in bytes): 11 + dest_connection_id_length + source_connection_id_length
}
