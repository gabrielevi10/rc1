
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

/*
Network topology:
    a1----|             |----b1
    a2----c1-----d-----c2----b2
    a3----|             |----b3
    a4----|             |----b4


*/

int main(int argc, char **argv){

    //LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_ALL);
    //LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_ALL);
    //LogComponentEnable("LiIonEnergySource", LOG_LEVEL_DEBUG);
    //LogComponentEnableAll(LOG_LEVEL_DEBUG);

    std::string outputFolder = "output/";

    NodeContainer nodes,rede_LAN_1, rede_LAN_2, rede_WAN;
    nodes.Create(11);
    rede_LAN_1.Add(nodes.Get(0));   // a1
    rede_LAN_1.Add(nodes.Get(1));   // a2
    rede_LAN_1.Add(nodes.Get(2));   // a3
    rede_LAN_1.Add(nodes.Get(3));   // a4
    rede_LAN_1.Add(nodes.Get(4));   // c1

    rede_LAN_2.Add(nodes.Get(5));   // b1  
    rede_LAN_2.Add(nodes.Get(6));   // b2
    rede_LAN_2.Add(nodes.Get(7));   // b3
    rede_LAN_2.Add(nodes.Get(8));   // b4
    rede_LAN_2.Add(nodes.Get(9));   // c2

    rede_WAN.Add(nodes.Get(10));    // d
    rede_WAN.Add(nodes.Get(4));     // c1 ---- d
    rede_WAN.Add(nodes.Get(9));     // c2 -----d => c1 ---- d ----- c2

    Ipv4StaticRoutingHelper staticRouting;
    OlsrHelper olsrRouting;

    Ipv4ListRoutingHelper routeHelper;
    routeHelper.Add(staticRouting, 0);
    routeHelper.Add(olsrRouting, 10);

    InternetStackHelper internetHelper, internetHelper2;
    internetHelper.SetRoutingHelper(routeHelper);

    internetHelper.Install(nodes.Get(4));
    internetHelper.Install(nodes.Get(9));
    internetHelper.Install(nodes.Get(10));

    internetHelper2.Install(nodes.Get(0));
    internetHelper2.Install(nodes.Get(1));
    internetHelper2.Install(nodes.Get(2));
    internetHelper2.Install(nodes.Get(3));
    internetHelper2.Install(nodes.Get(5));
    internetHelper2.Install(nodes.Get(6));
    internetHelper2.Install(nodes.Get(7));
    internetHelper2.Install(nodes.Get(8));
    
    // internetHelper.EnablePcapIpv4All(outputFolder+"pcap");

    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", StringValue("5ms"));

    
    NetDeviceContainer network_LAN_1, network_LAN_2, network_WAN; 
    PointToPointHelper pointHelper;
    pointHelper.SetChannelAttribute("Delay",StringValue("5ms"));
    
    // NetDeviceContainer csmaDevices;
    network_LAN_1 = csma.Install (rede_LAN_1);
    network_LAN_2 = csma.Install (rede_LAN_2);

    // // Cria conexões físicas entre a1 e c1
    // network_LAN_1 = pointHelper.Install(rede_LAN_1.Get(0), rede_LAN_1.Get(4));

    // // Cria conexões físicas entre a2 e c1
    // network_LAN_1 = pointHelper.Install(rede_LAN_1.Get(1), rede_LAN_1.Get(4));

    // // Cria conexões físicas entre a3 e c1
    // network_LAN_1 = pointHelper.Install(rede_LAN_1.Get(2), rede_LAN_1.Get(4));

    // // Cria conexões físicas entre a4 e c1
    // network_LAN_1 = pointHelper.Install(rede_LAN_1.Get(3), rede_LAN_1.Get(4));

    // // Cria conexões físicas entre b1 e c2
    // network_LAN_2 = pointHelper.Install(rede_LAN_2.Get(0), rede_LAN_2.Get(4));

    // // Cria conexões físicas entre b2 e c2
    // network_LAN_2 = pointHelper.Install(rede_LAN_2.Get(1), rede_LAN_2.Get(4));

    // // Cria conexões físicas entre b3 e c2
    // network_LAN_2 = pointHelper.Install(rede_LAN_2.Get(2), rede_LAN_2.Get(4));

    // // Cria conexões físicas entre b4 e c2
    // network_LAN_2 = pointHelper.Install(rede_LAN_2.Get(3), rede_LAN_2.Get(4));

    network_WAN = pointHelper.Install(rede_WAN.Get(0), rede_WAN.Get(1));
    network_WAN = pointHelper.Install(rede_WAN.Get(0), rede_WAN.Get(2));
   
    Ipv4InterfaceContainer interface_LAN_1, interface_LAN_2, interface_WAN;
    Ipv4AddressHelper ipv4Helper;

    //Configura endereços de rede das interfaces de rede do primeiro container
    ipv4Helper.SetBase("192.168.1.0","255.255.255.0");
    interface_LAN_1 = ipv4Helper.Assign(network_LAN_1);
    ipv4Helper.NewNetwork();

    //Configura endereços de rede das interfaces de rede do segundo container
    ipv4Helper.SetBase("192.168.2.0","255.255.255.0");
    interface_LAN_2 = ipv4Helper.Assign(network_LAN_2);
    ipv4Helper.NewNetwork();

    ipv4Helper.SetBase("192.168.10.0", "255.255.255.0");
    interface_WAN = ipv4Helper.Assign(network_WAN);
    ipv4Helper.NewNetwork();

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    //Output de todos point-to-point para pcap
    //pointHelper.EnablePcapAll(outputFolder+"pcap", true);

    ApplicationContainer clientApp, serverApp;

    UdpEchoServerHelper echoServerHelper(8080);
    serverApp = echoServerHelper.Install(rede_LAN_2.Get(0)); //Instala servidor echo em 192.168.1.0 ou 1

    UdpEchoClientHelper echoClientHelper(rede_LAN_2.Get(0)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(),8080);
    clientApp = echoClientHelper.Install(rede_LAN_2.Get(2)); //Instala cliente echo em 192.168.1.1 ou 2

    serverApp.Start(Seconds(1));
    serverApp.Stop(Seconds(30));
    clientApp.Start(Seconds(2));
    clientApp.Stop(Seconds(29));

    //Exportar simulação para netanim

    AnimationInterface anim(outputFolder+"animacaodotrabson.xml");
    
    anim.SetConstantPosition(nodes.Get(0), 12.5, 25);
    anim.SetConstantPosition(nodes.Get(1), 12.5, 37.5);
    anim.SetConstantPosition(nodes.Get(2), 12.5, 50);
    anim.SetConstantPosition(nodes.Get(3), 12.5, 62.5);
    anim.SetConstantPosition(nodes.Get(4), 25.5, 43.75);

    anim.SetConstantPosition(nodes.Get(5), 75, 25);
    anim.SetConstantPosition(nodes.Get(6), 75, 37.5);
    anim.SetConstantPosition(nodes.Get(7), 75, 50);
    anim.SetConstantPosition(nodes.Get(8), 75, 62.5);
    anim.SetConstantPosition(nodes.Get(9), 62.5, 43.75);

    anim.SetConstantPosition(nodes.Get(10), 42.5, 43.75);

    BaseStationNetDevice b;
    SubscriberStationNetDevice s;
    CsmaNetDevice c;
    UanNetDevice u;

    anim.SetMaxPktsPerTraceFile(0xFFFFFFFF);
    anim.EnablePacketMetadata(true);
    anim.EnableIpv4RouteTracking (outputFolder+"routingtable-wirelessTRABSON.xml", 
                                    Seconds (0), Seconds (9), Seconds (0.25));


    //Executar simulação por 10 segundos e depois destruir
    Simulator::Stop(Seconds(30));
    Simulator::Run();
    Simulator::Destroy();
}