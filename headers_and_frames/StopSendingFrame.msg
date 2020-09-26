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
class StopSendingFrame extends QuicFrame {
//An endpoint uses a STOP_SENDING frame (type=0x05) to communicate that incoming data is being discarded on
//receipt at application request. STOP_SENDING requests that a peer cease transmission on a stream.(RFC 19.5)
    int stream_id; // stream being ignored
    int error_code; // application protocol error code that indicates the reason the sender is ignoring the stream
}
