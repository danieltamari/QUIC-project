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
import inet.applications.contract.IApp;
import inet.applications.quicapp.IQuicapp;


simple ConnectionManager like IQuicapp
{

    parameters:
        //string destAddresses = default(""); // list of IP addresses, separated by spaces ("": don't send)
        //string localAddress = default("");
        string destAddresses; // space-separated list of destination IP addresses, can be empty;
                              // see module description for possibilities
        bool excludeLocalDestAddresses = default(true);     // excluding local addresses from destAddresses
        //string chooseDestAddrMode @enum("once","perBurst","perSend"); // when to change the destination address
        int destAddrRNG = default(0); // random generator ID for selecting a destination address
        int localPort = default(-1);  // local UDP port number (-1: use ephemeral port)
        int destPort; // remote UDP port number
     //   int numApps = default(1);
     	@signal[snd_wnd];
     	@signal[num_bytes_sent];
     	@signal[bytes_sent_with_ret];
     	@signal[rtt];
     	@signal[latency1];
     	@signal[latency2];
     	@signal[current_new_sent_bytes];
     	@signal[current_total_sent_bytes];
     	@signal[stream0];
     	@signal[stream1];
     	@signal[stream2];
     	@signal[stream3];
     	@signal[stream4];
     	@signal[stream5];
     	@signal[stream6];
     	@signal[stream7];
     	@signal[stream8];
     	@signal[stream9];
        
        @statistic[send_window](title="send window"; source="snd_wnd"; record=vector; interpolationmode=sample-hold);   	
        @statistic[bytes_sent](title="num bytes sent"; source="num_bytes_sent"; record=vector; interpolationmode=sample-hold);
        @statistic[rtt](title="rtt"; source="rtt"; record=vector; interpolationmode=sample-hold);
        @statistic[latency1](title="latency1"; source="latency1"; record=vector; interpolationmode=sample-hold);
        @statistic[latency2](title="latency2"; source="latency2"; record=vector; interpolationmode=sample-hold);
        @statistic[bytes_sent_with_ret](title="num bytes sent with ret"; source="bytes_sent_with_ret"; record=vector; interpolationmode=sample-hold);
        @statistic[current_new_sent_bytes](title="current_new_sent_bytes"; source="current_new_sent_bytes"; record=vector; interpolationmode=sample-hold);
        @statistic[current_total_sent_bytes](title="current_total_sent_bytes"; source="current_total_sent_bytes"; record=vector; interpolationmode=sample-hold);
        @statistic[stream0](title="stream 0"; source="stream0"; record=vector; interpolationmode=sample-hold);
        @statistic[stream1](title="stream 1"; source="stream1"; record=vector; interpolationmode=sample-hold);
        @statistic[stream2](title="stream 2"; source="stream2"; record=vector; interpolationmode=sample-hold);
        @statistic[stream3](title="stream 3"; source="stream3"; record=vector; interpolationmode=sample-hold);
        @statistic[stream4](title="stream 4"; source="stream4"; record=vector; interpolationmode=sample-hold);
        @statistic[stream5](title="stream 5"; source="stream5"; record=vector; interpolationmode=sample-hold);
        @statistic[stream6](title="stream 6"; source="stream6"; record=vector; interpolationmode=sample-hold);
        @statistic[stream7](title="stream 7"; source="stream7"; record=vector; interpolationmode=sample-hold);
        @statistic[stream8](title="stream 8"; source="stream8"; record=vector; interpolationmode=sample-hold);
        @statistic[stream9](title="stream 9"; source="stream9"; record=vector; interpolationmode=sample-hold);
   
    gates:
        input socketIn @labels(UdpControlInfo/up);
        output socketOut @labels(UdpControlInfo/down);
        input quicIn[];
        //input toc_in;
        //output tic_out;
        //output toc_out;
}
