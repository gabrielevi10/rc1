
#include <ns3/core-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/internet-module.h>
#include <ns3/applications-module.h>

#include "ns3/olsr-helper.h"

#include <ns3/netanim-module.h>
#include <ns3/bs-net-device.h>
#include <ns3/csma-module.h>
#include <ns3/uan-module.h>


using namespace ns3;

int main(int argc, char **argv){

	std::string outputFolder = "output/";

	NodeContainer nodes, rede1, rede2, rede3, rede4, rede5, redeWAN;
	nodes.Create(9);
	rede1.Add(nodes.Get(1));
	rede1.Add(nodes.Get(2));

	rede2.Add(nodes.Get(3));
	rede2.Add(nodes.Get(4));

	rede3.Add(nodes.Get(5));
	rede3.Add(nodes.Get(6));

	rede4.Add(nodes.Get(7));
	rede4.Add(nodes.Get(8));

	redeWAN.Add(nodes.Get(0));
	redeWAN.Add(nodes.Get(1));
	redeWAN.Add(nodes.Get(3));
	redeWAN.Add(nodes.Get(5));
	redeWAN.Add(nodes.Get(7));

	Ipv4StaticRoutingHelper staticRouting;
    OlsrHelper olsrRouting;

	Ipv4ListRoutingHelper routeHelper;
    routeHelper.Add(staticRouting, 0);
    routeHelper.Add(olsrRouting, 10);

    InternetStackHelper internetHelper;
    internetHelper.SetRoutingHelper(routeHelper);
    internetHelper.Install(nodes);
    internetHelper.EnablePcapIpv4All(outputFolder+"pcap");


    NetDeviceContainer netLAN1, netLAN2, netLAN3, netLAN4, netWAN; 
    PointToPointHelper pointHelper;
    pointHelper.SetChannelAttribute("Delay",StringValue("5ms"));

    //Cria conexões físicas entre os dois nós do primeiro container ( roteador = nodes(1) = nodesRede1.Get(1) )
     netLAN1 = pointHelper.Install(rede1.Get(0),rede1.Get(1));

    //Cria conexões físicas entre os dois nós do segundo container ( roteador = nodes(1) = nodesRede2.Get(0) )
    netLAN2 = pointHelper.Install(rede2.Get(0),rede2.Get(1));

    //Cria conexões físicas entre os dois nós do segundo container ( roteador = nodes(2) = nodesRede3.Get(0) )
    netLAN3 = pointHelper.Install(rede3.Get(0),rede3.Get(1));

    netLAN4 = pointHelper.Install(rede4.Get(0), rede4.Get(1));

    netWAN = pointHelper.Install(redeWAN.Get(0), redeWAN.Get(1));
    netWAN = pointHelper.Install(redeWAN.Get(0), redeWAN.Get(2));
    netWAN = pointHelper.Install(redeWAN.Get(0), redeWAN.Get(3));
    netWAN = pointHelper.Install(redeWAN.Get(0), redeWAN.Get(4));


    Ipv4InterfaceContainer interfaceLAN1, interfaceLAN2, interfaceLAN3, interfaceLAN4, interfaceWAN;
    Ipv4AddressHelper ipv4Helper;

    //Configura endereços de rede das interfaces de rede do primeiro container
    ipv4Helper.SetBase("192.168.1.0","255.255.255.0");
    interfaceLAN1 = ipv4Helper.Assign(netLAN1);
    ipv4Helper.NewNetwork();

    //Configura endereços de rede das interfaces de rede do segundo container
    ipv4Helper.SetBase("192.168.2.0","255.255.255.0");
    interfaceLAN2 = ipv4Helper.Assign(netLAN2);
    ipv4Helper.NewNetwork();

    //Configura endereços de rede das interfaces de rede do terceiro container
    ipv4Helper.SetBase("192.168.3.0","255.255.255.0");
    interfaceLAN3 = ipv4Helper.Assign(netLAN3);
    ipv4Helper.NewNetwork();

    ipv4Helper.SetBase("192.168.4.0","255.255.255.0");
    interfaceLAN4 = ipv4Helper.Assign(netLAN4);
    ipv4Helper.NewNetwork();

    ipv4Helper.SetBase("192.168.100.0", "255.255.255.0");
    interfaceWAN = ipv4Helper.Assign(netWAN);
    ipv4Helper.NewNetwork();

    //Output de todos point-to-point para pcap
    //pointHelper.EnablePcapAll(outputFolder+"pcap", true);

    //Exportar simulação para netanim

    AnimationInterface anim(outputFolder+"animacaodotrabson.xml");
    
    anim.SetConstantPosition(nodes.Get(0), 50, 50);
    anim.SetConstantPosition(nodes.Get(1), 25, 25);
    anim.SetConstantPosition(nodes.Get(2), 25, 35);
    anim.SetConstantPosition(nodes.Get(3), 75, 25);
    anim.SetConstantPosition(nodes.Get(4), 75, 35);
    anim.SetConstantPosition(nodes.Get(5), 25, 75);
    anim.SetConstantPosition(nodes.Get(6), 25, 85);
    anim.SetConstantPosition(nodes.Get(7), 75, 75);
    anim.SetConstantPosition(nodes.Get(8), 75, 85);

    BaseStationNetDevice b;
    SubscriberStationNetDevice s;
    CsmaNetDevice c;
    UanNetDevice u;

    anim.SetMaxPktsPerTraceFile(0xFFFFFFFF);
    anim.EnablePacketMetadata(true);
    anim.EnableIpv4RouteTracking (outputFolder+"routingtable-wirelessTRABSON.xml", Seconds (0), Seconds (9), Seconds (0.25));


    //Executar simulação por 10 segundos e depois destruir
    Simulator::Stop(Seconds(30));
    Simulator::Run();
    Simulator::Destroy();
}