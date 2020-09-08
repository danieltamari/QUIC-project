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
        client1: QuicClient {
            parameters:
                @display("p=199,100;i=device/pc3");
        }
//        client2: QuicClient {
//            parameters:
//                @display("p=199,175;i=device/pc3");
//        }
        server: QuicServer {
            parameters:
                @display("p=331,100;i=device/pc2");
        }
//        server2: QuicServer {
//            parameters:
//                @display("p=341,168;i=device/pc2");
//        }

        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s");
        }
//        router: Router {
//            @display("p=254,127");
//        }
    connections allowunconnected:

        client1.pppg++ <--> C <--> server.pppg++;
        //client1.pppg++ <--> C <--> router.pppg++;
        //client2.pppg++ <--> C <--> router.pppg++;
        //server.pppg++ <--> C <--> router.pppg++;
        //server2.pppg++ <--> C <--> router.pppg++;





}
