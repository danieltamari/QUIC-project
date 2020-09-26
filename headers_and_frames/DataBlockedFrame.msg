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
class DataBlockedFrame extends QuicFrame  {
//A sender SHOULD send a DATA_BLOCKED frame (type=0x14) when it wishes to send data, but is unable to do so
//due to connection-level flow control; DATA_BLOCKED frames can be used as input to tuning of flow control 
//algorithms; (RFC 19.12)	
	int maximum_data; // the connection-level limit at which blocking occurred
    }