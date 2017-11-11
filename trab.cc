#include <ns3/core-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/internet-module.h>
#include <ns3/applications-module.h>


//Para netanim
#include <ns3/netanim-module.h>
#include <ns3/bs-net-device.h>
#include <ns3/csma-module.h>
#include <ns3/uan-module.h>

/*
Network topology:

n1 n2 n3 n4 ------- n0 ------- n5 n6 n7 n8  
|  |  |  |          WAN        |  |  |  |           
==========                     ==========
    LAN                           LAN
*/

using namespace ns3;

int main(int argc, char **argv){

// Criando os nós n0 e n4 que se conectam por fora da LAN
    NodeContainer pNodes1;
    pNodes1.Create(2);

// Criando a LAN
    NodeContainer csmaNodes1;
    csmaNodes1.Add(pNodes1.Get(1));
    csmaNodes1.Create(3);

    PointToPointHelper pHelper1;
    pHelper1.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
    pHelper1.SetChannelAttribute("Delay", StringValue ("2ms"));

    NetDeviceContainer pNDevice1;
    pNDevice1 = pHelper1.Install(pNodes1);
    
    CsmaHelper csma1;
    csma1.SetChannelAttribute("DataRate", StringValue ("100Mbps"));
    csma1.SetChannelAttribute("Delay", TimeValue (NanoSeconds (6560)));

    NetDeviceContainer csmaDev1;
    csmaDev1 = csma1.Install(csmaNodes1);

    InternetStackHelper stack;
    stack.Install(pNodes1.Get (0));
    stack.Install(csmaNodes1);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer pInterfaces;
    pInterfaces = address.Assign(pNDevice1);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDev1);

    UdpEchoServerHelper echoServer (8000);

    ApplicationContainer serverApps = echoServer.Install(csmaNodes1.Get(3));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (10.0));

    UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress(3), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.)));
    echoClient.SetAttribute("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApps = echoClient.Install(pNodes1.Get(0));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    pHelper1.EnablePcapAll("second");
    csma1.EnablePcap("second", csmaDev1.Get(1), true);

// para utilizar o NetAnim
    BaseStationNetDevice b;
    SubscriberStationNetDevice s;
    CsmaNetDevice k;
    UanNetDevice u;


    AnimationInterface anim("output/animtrab1.xml");
    anim.SetMaxPktsPerTraceFile(0xFFFFFFFF);
    anim.EnablePacketMetadata(true);
    anim.EnableIpv4RouteTracking ("output/routingtable-wireless1.xml", Seconds (0), Seconds (9), Seconds (0.25));

// Comecar a simulacao
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}