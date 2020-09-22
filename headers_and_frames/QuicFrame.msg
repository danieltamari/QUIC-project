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
namespace inet;

//
// TODO generated message class
//
class QuicFrame extends FieldsChunk {
	int frame_type;
//	     +-------------+----------------------+---------------+---------+
//     | Type Value  | Frame Type Name      | Definition    | Packets |
//     +=============+======================+===============+=========+
//     | 0x00        | PADDING              | Section 19.1  | IH01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x01        | PING                 | Section 19.2  | IH01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x02 - 0x03 | ACK                  | Section 19.3  | IH_1    |
//     +-------------+----------------------+---------------+---------+
//     | 0x04        | RESET_STREAM         | Section 19.4  | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x05        | STOP_SENDING         | Section 19.5  | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x06        | CRYPTO               | Section 19.6  | IH_1    |
//     +-------------+----------------------+---------------+---------+
//     | 0x07        | NEW_TOKEN            | Section 19.7  | ___1    |
//     +-------------+----------------------+---------------+---------+
//     | 0x08 - 0x0f | STREAM               | Section 19.8  | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x10        | MAX_DATA             | Section 19.9  | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x11        | MAX_STREAM_DATA      | Section 19.10 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x12 - 0x13 | MAX_STREAMS          | Section 19.11 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x14        | DATA_BLOCKED         | Section 19.12 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x15        | STREAM_DATA_BLOCKED  | Section 19.13 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x16 - 0x17 | STREAMS_BLOCKED      | Section 19.14 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x18        | NEW_CONNECTION_ID    | Section 19.15 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x19        | RETIRE_CONNECTION_ID | Section 19.16 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x1a        | PATH_CHALLENGE       | Section 19.17 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x1b        | PATH_RESPONSE        | Section 19.18 | __01    |
//     +-------------+----------------------+---------------+---------+
//     | 0x1c - 0x1d | CONNECTION_CLOSE     | Section 19.19 | IH_1*   |
//     +-------------+----------------------+---------------+---------+
//     | 0x1e        | HANDSHAKE_DONE       | Section 19.20 | ___1    |
//     +-------------+----------------------+---------------+---------+
}
