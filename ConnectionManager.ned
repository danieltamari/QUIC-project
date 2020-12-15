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
        int localPort = default(-1);  // local UDP port number (-1: use ephemeral port)
        int destPort; // remote UDP port number
        int max_payload=default(0);
        int init_stream_flow_control_window = default(0);
        int init_connection_flow_control_winodw =default(0);
        double max_ack_delay=default(0);
        @signal[snd_wnd];
        @signal[num_bytes_sent];
        @signal[bytes_sent_with_ret];
        @signal[rtt];
        @signal[latency1];
        @signal[latency2];
        @signal[latency3];
        @signal[latency4];
        @signal[current_new_sent_bytes];
        @signal[current_total_sent_bytes];
        @signal[current_new_sent_bytes_for_long];
        @signal[current_total_sent_bytes_for_long];
        @signal[total_bytes_in_curr_send];
        @signal[new_bytes_in_curr_send];
        @signal[throughput1];
        @signal[throughput2];
        @signal[throughput3];
        @signal[throughput4];
        @signal[throughput1_long];
        @signal[throughput2_long];
        @signal[throughput3_long];
        @signal[throughput4_long];


        @statistic[send_window](title="send window"; source="snd_wnd"; record=vector; interpolationmode=sample-hold);
        @statistic[bytes_sent](title="num bytes sent"; source="num_bytes_sent"; record=vector; interpolationmode=sample-hold);
        @statistic[rtt](title="rtt"; source="rtt"; record=vector; interpolationmode=sample-hold);
        @statistic[latency1](title="latency1"; source="latency1"; record=vector; interpolationmode=sample-hold);
        @statistic[latency2](title="latency2"; source="latency2"; record=vector; interpolationmode=sample-hold);
        @statistic[latency3](title="latency3"; source="latency3"; record=vector; interpolationmode=sample-hold);
        @statistic[latency4](title="latency4"; source="latency4"; record=vector; interpolationmode=sample-hold);
        @statistic[bytes_sent_with_ret](title="num bytes sent with ret"; source="bytes_sent_with_ret"; record=vector; interpolationmode=sample-hold);
        @statistic[current_new_sent_bytes](title="current_new_sent_bytes"; source="current_new_sent_bytes"; record=vector; interpolationmode=sample-hold);
        @statistic[current_total_sent_bytes](title="current_total_sent_bytes"; source="current_total_sent_bytes"; record=vector; interpolationmode=sample-hold);
        @statistic[total_bytes_in_curr_send](title="total_bytes_in_curr_send"; source="total_bytes_in_curr_send"; record=vector; interpolationmode=sample-hold);
        @statistic[new_bytes_in_curr_send](title="new_bytes_in_curr_send"; source="new_bytes_in_curr_send"; record=vector; interpolationmode=sample-hold);
        @statistic[current_new_sent_bytes_for_long](title="current_new_sent_bytes_for_long"; source="current_new_sent_bytes_for_long"; record=vector; interpolationmode=sample-hold);
        @statistic[current_total_sent_bytes_for_long](title="current_total_sent_bytes_for_long"; source="current_total_sent_bytes_for_long"; record=vector; interpolationmode=sample-hold);

        @statistic[throughput1](title="throughput1"; source="throughput1"; record=vector; interpolationmode=sample-hold);
        @statistic[throughput2](title="throughput2"; source="throughput2"; record=vector; interpolationmode=sample-hold);
        @statistic[throughput3](title="throughput3"; source="throughput3"; record=vector; interpolationmode=sample-hold);
        @statistic[throughput4](title="throughput1"; source="throughput4"; record=vector; interpolationmode=sample-hold);
        @statistic[throughput1_long](title="throughput1_long"; source="throughput1_long"; record=vector; interpolationmode=sample-hold);
        @statistic[throughput2_long](title="throughput2_long"; source="throughput2_long"; record=vector; interpolationmode=sample-hold);
        @statistic[throughput3_long](title="throughput3_long"; source="throughput3_long"; record=vector; interpolationmode=sample-hold);
        @statistic[throughput4_long](title="throughput4_long"; source="throughput4_long"; record=vector; interpolationmode=sample-hold);

    gates:
        input socketIn @labels(UdpControlInfo/up);
        output socketOut @labels(UdpControlInfo/down);
        input quicIn[];

}
