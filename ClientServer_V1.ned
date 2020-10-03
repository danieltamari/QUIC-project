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

network ClientServer_V1
{
    parameters:
        double per = default(0);
        @display("bgb=553,335");
    types:
        channel C extends DatarateChannel
        {
            datarate = 10Mbps;
            delay = 0.1us;
            per = per;
        }

        channel C2 extends DatarateChannel
        {
            datarate = 2Mbps;
            delay = 0.1us;
            per = per;
        }

    submodules:
        client1: QuicOneClient {
            parameters:
                @display("p=103,38;i=device/pc3");
        }
        server: QuicServer {
            parameters:
                @display("p=458,69;i=device/pc2");
        }
        client2: QuicOneClient {
            parameters:
                @display("p=103,108;i=device/pc3");
        }
        client3: QuicOneClient {
            parameters:
                @display("p=103,195;i=device/pc3");
        }
        client4: QuicOneClient {
            parameters:
                @display("p=103,261;i=device/pc3");
        }
        server2: QuicServer {
            parameters:
                @display("p=458,156;i=device/pc2");
        }
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=29,100;is=s");
        }
        router: Router {
            @display("p=282,135");
        }

    connections allowunconnected:


        client1.pppg++ <--> C <--> router.pppg++;
        server.pppg++ <--> C <--> router.pppg++;
        client2.pppg++ <--> C <--> router.pppg++;
        server2.pppg++ <--> C <--> router.pppg++;
        client3.pppg++ <--> C <--> router.pppg++;
        client4.pppg++ <--> C <--> router.pppg++;

}
