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
class StreamFrame extends QuicFrame {
//STREAM frames implicitly create a stream and carry stream data. The STREAM frame Type field takes the form
//0b00001XXX(values from 0x08 to 0x0f). The three low-order bits of the frame type determine the fields that 
//are present in the frame:
//The OFF bit (0x04) in the frame type is set to indicate that there is an Offset field present. When set to
//	 0, the Offset field is absent and the Stream Data starts at an offset of 0 (frame contains the first 
//	 bytes of the stream, or the end of a stream that includes no data).
//The LEN bit (0x02) in the frame type is set to indicate that there is a Length field present. If this bit
//	 is set to 0, the Length field is absent and the Stream Data field extends to the end of the packet
//The FIN bit (0x01) of the frame type is set only on frames that contain the final size of the stream. 
//	Setting this bit indicates that the frame marks the end of the stream. (RFC 19.8)

	int stream_id; // stream ID of the stream
    int offset; // the byte offset in the stream for the data in this STREAM frame
    int length; // length of the Stream Data field in this STREAM frame
    bool is_FIN;
}
