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
class ConnectionCloseFrame extends QuicFrame  {
//An endpoint sends a CONNECTION_CLOSE frame (type=0x1c or 0x1d) to notify its peer that the connection is
//being closed. The CONNECTION_CLOSE with a frame type of 0x1c is used to signal errors at only the QUIC 
//layer, or the absence of errors (with the NO_ERROR code). The CONNECTION_CLOSE frame with a type of 0x1d 
//is used to signal an error with the application that uses QUIC.(RFC 19.19)
	int error_code; // indicates the reason for closing this connection
	int frame_type; // frame that triggered the error. A value of 0 is used when the frame type is unknown
	char reason_pharse[]; // A human-readable explanation for why the connection was closed
	int reason_pharse_length; // length of the reason phrase in bytes
    }