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

package inet.applications.quicapp;


simple SimV1_client
{
    parameters:
        //int number_of_streams;
        //int stream_size;
        string streams_list;
        string connectAddress;
        double tSend @unit(s) = default(0s);
        int sim_type; // 1 = SENDER , 3=ADDSTREAMS
        //int my_client_number;
    gates:
        input in;
        output out;
}