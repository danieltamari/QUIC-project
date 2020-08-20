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
import inet.node.inet.StandardHost;
import ned.DatarateChannel;

import inet.common.misc.NetAnimTrace;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

//
// TODO auto-generated type
//






//simple SimV1_server
//{
//    gates:
//        input in;
//        output out;
//}

//simple QuicConnection
//{
//    gates:
//        input tic_in;
//        input toc_in;
//        output tic_out;
//        output toc_out;
//}
        //ticConnection: QuicConnection;


//        client1: StandardHost {
//            parameters:
//                @display("p=250,100;i=device/pc3");
//        }
//        tic.out --> {  delay = 100ms; } --> ticConnection.tic_in;
//        ticConnection.toc_out --> {  delay = 100ms; } --> toc.in;
//        tic.in <-- {  delay = 100ms; } <-- ticConnection.tic_out;
//        ticConnection.toc_in <-- {  delay = 100ms; } <-- toc.out;


network QuicV1
{
    parameters:
        double per = default(0);
    types:
        channel C extends DatarateChannel
        {
            datarate = 10Mbps;
            delay = 0.1us;
            per = per;
        }

    submodules:
        //    tic: SimV1_client {
        //         @display("p=30,158");
        //     }
        //     toc: SimV1_server {
        //        @display("p=209,44");
        //    }
        client1: QuicClient {
            parameters:
                @display("p=199,100;i=device/pc3");
        }

        server: QuicServer {
            parameters:
                @display("p=331,100;i=device/pc2");
        }

        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s");
        }
    connections allowunconnected:


        //     tic.out --> client1.ClientIn;
        // tic.in <-- {  delay = 100ms; } <-- client1.ClientOut;
        //      toc.out --> {  delay = 100ms; } --> server.ClientIn;
        //toc.in <-- {  delay = 100ms; } <-- server.ClientOut;
        client1.pppg++ <--> C <--> server.pppg++;
}
