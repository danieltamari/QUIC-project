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
class MaxStreamsFrame extends QuicFrame  {
//MAX_STREAMS frame (type=0x12 or 0x13) inform the peer of the cumulative number of streams of a given type
//it is permitted to open. (RFC 19.11)
	int maximum_streams; // A count of the cumulative number of streams that can be opened over the 
	//lifetime of the connection. This value cannot exceed 2^60
    }