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
import inet.node.bgp.BgpRouter;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;

import inet.common.misc.NetAnimTrace;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

//
// TODO auto-generated type
//

network QuicHighLossNetwork
{
    parameters:
        double per = default(0.01);
        @display("bgb=668,191");
    types:
        channel C extends DatarateChannel
        {
            datarate = 10Mbps;
            //delay = 0.1us;
            delay = 1ms;
            per = per;
        }

//        channel C2 extends DatarateChannel
//        {
//            datarate = 2Mbps;
//            delay = 0.1us;
//            per = per;
//        }

    submodules:
        client1: QuicOneClient {
            parameters:
                @display("p=131,50;i=device/pc3");
        }

        server: QuicServer {
            parameters:
                @display("p=640,62;i=device/pc2");
        }

        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=39,150;is=s");
        }
        router: Router {
            @display("p=356,154");
        }
    connections allowunconnected:


        client1.pppg++ <--> C <--> router.pppg++;
        router.pppg++ <--> C <--> server.pppg++;


}