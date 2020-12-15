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
import inet.node.internetcloud.InternetCloud;

//
// TODO auto-generated type
//

network ZeroRttNetwork
{
    parameters:
        double per = default(0);
        @display("bgb=654,289");
    types:
        channel C extends DatarateChannel
        {
            datarate = 10Mbps;
            delay = 50ms;
            per = per;
        }

    submodules:
        client1: QuicTwoClient {
            parameters:
                @display("p=103,69;i=device/pc3");
        }
        server: QuicServer {
            parameters:
                @display("p=591,100;i=device/pc2");
        }
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=29,100;is=s");
        }
        router: Router {
            @display("p=362,116");
        }

    connections allowunconnected:

        client1.pppg++ <--> C <--> router.pppg++;

        router.pppg++ <--> C <--> server.pppg++;


}
