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
import inet.linklayer.ethernet.EtherHub;
import inet.node.bgp.BgpRouter;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.MulticastRouter;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;

import inet.common.misc.NetAnimTrace;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.internetcloud.InternetCloud;



network QuicLargeNetwork
{
    parameters:
        double per = default(0.01);

        @display("bgb=1013.02496,677.62494");
    types:
        channel C extends DatarateChannel
        {
            datarate = 10Mbps;
            delay = 20ms;
            per = per;
        }

        channel C2 extends DatarateChannel
        {
            datarate = 2Mbps;
            delay = 20ms;
            per = per;
        }
        channel C3 extends DatarateChannel
        {
            datarate = 10Mbps;
            delay = 1us;
           // per = 0;
        }


    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=51.674995,37.05");
        }
        server_local_net1: QuicServer {
            @display("p=522,88;i=device/pc2");
        }
        server_local_net2: QuicServer {
            @display("p=522,601;i=device/pc2");
        }
        server_remote: QuicServer {
            @display("p=830,347;i=device/pc2");
        }


        //		### Network1 ###
        client1_net1: QuicOneClient {
            @display("p=183,40;i=device/pc3");
        }
        client2_net1: QuicOneClient {
            @display("p=183,110;i=device/pc3");
        }
        switch1_net1: EtherSwitch {
            @display("p=300,75");
        }

        client3_net1: QuicOneClient {
            @display("p=183,180;i=device/pc3");
        }
        client4_net1: QuicOneClient {
            @display("p=183,250;i=device/pc3");
        }
        switch2_net1: EtherSwitch {
            @display("p=300,215");
        }
        router_local_net1: Router {
            @display("p=404,156");
        }

        //		### Network2 ###
        client1_net2: QuicOneClient {
            @display("p=183,400;i=device/pc3");
        }
        client2_net2: QuicOneClient {
            @display("p=183,470;i=device/pc3");
        }
        switch1_net2: EtherSwitch {
            @display("p=300,435");
        }
        client3_net2: QuicOneClient {
            @display("p=183,540;i=device/pc3");
        }
        client4_net2: QuicOneClient {
            @display("p=183,610;i=device/pc3");
        }
        switch2_net2: EtherSwitch {
            @display("p=300,575");
        }
        router_local_net2: Router {
            @display("p=404,517");
        }

        remoteRouter: Router { // predefined router
            @display("p=571,347");
        }
        internet: InternetCloud {
            @display("p=696,346;is=vl");
        }

    connections: // network level connections
        client1_net1.ethg++ <--> C <--> switch1_net1.ethg++; // 
        client2_net1.ethg++ <--> C <--> switch1_net1.ethg++; //

        client3_net1.ethg++ <--> C <--> switch2_net1.ethg++; // 
        client4_net1.ethg++ <--> C <--> switch2_net1.ethg++; // 

        router_local_net1.ethg++ <--> C <--> switch1_net1.ethg++; // 
        router_local_net1.ethg++ <--> C <--> switch2_net1.ethg++; // 

        client1_net2.ethg++ <--> C <--> switch1_net2.ethg++; // 
        client2_net2.ethg++ <--> C <--> switch1_net2.ethg++; //

        client3_net2.ethg++ <--> C <--> switch2_net2.ethg++; // 
        client4_net2.ethg++ <--> C <--> switch2_net2.ethg++; // 

        router_local_net2.ethg++ <--> C <--> switch1_net2.ethg++; // 
        router_local_net2.ethg++ <--> C <--> switch2_net2.ethg++; // 

        router_local_net1.pppg++ <--> C2 <--> server_local_net1.pppg++; // 
        router_local_net2.pppg++ <--> C2 <--> server_local_net2.pppg++; // 



        router_local_net1.pppg++ <--> C3 <--> remoteRouter.pppg++; // 
        router_local_net2.pppg++ <--> C3 <--> remoteRouter.pppg++; // 


        remoteRouter.pppg++ <--> C3 <--> internet.pppg++; // 
        internet.pppg++ <--> C3 <--> server_remote.pppg++; // 

}
