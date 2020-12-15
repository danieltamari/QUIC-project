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
import inet.applications.quicapp.ConnectionManager;
import inet.applications.quicapp.Client_Module;
import inet.node.inet.StandardHost;
import inet.node.base.TransportLayerNodeBase;
import inet.applications.quicapp.IQuicapp;
import inet.common.MessageDispatcher;




module QuicThreeClient extends TransportLayerNodeBase
{
    submodules:
        connection_manager: <> like IQuicapp {
            parameters:
                @display("p=825.3,50.399998,row,150");
        }



        trigger1: Client_Module {
            @display("p=974.925,50.399998");
        }

        trigger2: Client_Module {
            @display("p=960.74994,127.575");
        }
        trigger3: Client_Module {
            @display("p=875.69995,189");
        }
        at: MessageDispatcher {
            parameters:
                @display("p=750,146;b=1000,5,,,,1");
        }


    connections allowunconnected:
        trigger1.out --> connection_manager.quicIn++;
        trigger2.out --> connection_manager.quicIn++;
        trigger3.out --> connection_manager.quicIn++;


        connection_manager.socketOut --> at.in++;
        connection_manager.socketIn <-- at.out++;

        at.out++ --> udp.appIn if hasUdp;
        at.in++ <-- udp.appOut if hasUdp;

        at.out++ --> tcp.appIn if hasTcp;
        at.in++ <-- tcp.appOut if hasTcp;

        at.out++ --> sctp.appIn if hasSctp;
        at.in++ <-- sctp.appOut if hasSctp;

        at.out++ --> tn.in++;
        at.in++ <-- tn.out++;

}
