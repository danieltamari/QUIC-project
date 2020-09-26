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
class ResetStreamFrame extends QuicFrame {
//An endpoint uses a RESET_STREAM frame (type=0x04) to abruptly terminate the sending part of a stream.
//After sending a RESET_STREAM, an endpoint ceases transmission and retransmission of STREAM frames on
//the identified stream. A receiver of RESET_STREAM can discard any data that it already received on that 
//stream. (RFC 19.4)
    int stream_id; // stream being terminated
    int final_size; // final size of the stream by the RESET_STREAM sender, in unit of bytes
    int error_code; // application protocol error code that indicates why the stream is being closed
}
