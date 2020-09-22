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
import inet.applications.quicapp.headers_and_frames.QuicFrame;
namespace inet;


//
// TODO generated message class
//

class QuicHandShakeData extends QuicFrame {
    // Transport parameters exchanged during Handshake
    
    // The value that the endpoint included in the Source Connection ID field of the first
    // Initial packet it sends for the connection
	int initial_source_connection_id; // source_ID initiated by client

    // The value of the Destination Connection ID field from the first Initial packet sent
    // by the client; This transport parameter is only sent by a server. (RFC 18.2)
	int original_destination_connection_id; // inital Dest_ID server get in the first packet
	
	// The value that the server included in the Source Connection ID field of a Retry packet;
	// This transport parameter is only sent by a server.
	int retry_source_connection_id; // new Source_ID negotiated by server
	
	// limits the size of UDP payloads that the endpoint is willing to receive. UDP packets with
    // payloads larger than this limit are not likely to be processed by the receiver. (RFC 18.2)
	int max_udp_payload_size;
	
	// contains the initial value for the maximum amount of data that can be sent on the 
	// connection. This is equivalent to sending a MAX_DATA for the connection immediately
	// after completing the handshake.
	int initial_max_data; 
	
	// specifying the initial flow control limit for locally-initiated bidirectional streams. 
	int initial_max_stream_data;
	

    
}
