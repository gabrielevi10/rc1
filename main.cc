
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
                    a1=|                                      |=c1
                    a2=|__l1                              l3__|=c2
lan1: 192.10.10.0   a3=|    \                             /   |=c3 lan3: 192.20.10.0
                    a4=|     \                           /    |=c4
                              w1-----------w3----------w2
                             /                          \
                    b1=|    /                            \    |=d1
lan2: 192.10.20.0   b2=|__l2                              l4__|=d2
                    b3=|                                      |=d3 lan4: 192.20.20.0
                    b4=|                                      |=d4


*/

int main(int argc, char **argv){

    std::string outputFolder = "output/";

    NodeContainer nodes, csma1, csma2, csma3, csma4;
    nodes.Create(7);

    csma1.Add(nodes.Get(0));
    csma1.Create(4);

    csma2.Add(nodes.Get(1));
    csma2.Create(4);

    csma3.Add(nodes.Get(3));
    csma3.Create(4);

    csma4.Add(nodes.Get(4));
    csma4.Create(4);

    AnimationInterface::SetConstantPosition(nodes.Get(0), 25, 25);
    AnimationInterface::SetConstantPosition(nodes.Get(1), 25, 75);
    AnimationInterface::SetConstantPosition(nodes.Get(2), 50, 50);
    AnimationInterface::SetConstantPosition(nodes.Get(3), 75, 25);
    AnimationInterface::SetConstantPosition(nodes.Get(4), 75, 75);
    AnimationInterface::SetConstantPosition(nodes.Get(5), 37.5, 50);
    AnimationInterface::SetConstantPosition(nodes.Get(6), 62.5, 50);

    AnimationInterface::SetConstantPosition(csma1.Get(1), 12.5, 15);
    AnimationInterface::SetConstantPosition(csma1.Get(2), 12.5, 20);
    AnimationInterface::SetConstantPosition(csma1.Get(3), 12.5, 25);
    AnimationInterface::SetConstantPosition(csma1.Get(4), 12.5, 30);
    
    AnimationInterface::SetConstantPosition(csma2.Get(1), 12.5, 65);
    AnimationInterface::SetConstantPosition(csma2.Get(2), 12.5, 70);
    AnimationInterface::SetConstantPosition(csma2.Get(3), 12.5, 75);
    AnimationInterface::SetConstantPosition(csma2.Get(4), 12.5, 80);

    AnimationInterface::SetConstantPosition(csma3.Get(1), 87.5, 15);
    AnimationInterface::SetConstantPosition(csma3.Get(2), 87.5, 20);
    AnimationInterface::SetConstantPosition(csma3.Get(3), 87.5, 25);
    AnimationInterface::SetConstantPosition(csma3.Get(4), 87.5, 30);

    AnimationInterface::SetConstantPosition(csma4.Get(1), 87.5, 65);
    AnimationInterface::SetConstantPosition(csma4.Get(2), 87.5, 70);
    AnimationInterface::SetConstantPosition(csma4.Get(3), 87.5, 75);
    AnimationInterface::SetConstantPosition(csma4.Get(4), 87.5, 80);


    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

    NetDeviceContainer lan1, lan2, lan3, lan4;

    lan1 = csma.Install(csma1);
    lan2 = csma.Install(csma2);
    lan3 = csma.Install(csma3);
    lan4 = csma.Install(csma4);

    Ipv4StaticRoutingHelper staticRouting;
    OlsrHelper olsrRouting;

    Ipv4ListRoutingHelper routeHelper;
    routeHelper.Add(staticRouting, 0);
    routeHelper.Add(olsrRouting, 10);

    InternetStackHelper stack;
    stack.SetRoutingHelper(routeHelper);
    stack.Install(csma1);
    stack.Install(csma2);
    stack.Install(csma3);
    stack.Install(csma4);

    stack.Install(nodes.Get(2));
    stack.Install(nodes.Get(5));
    stack.Install(nodes.Get(6));

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer p2p1, p2p2, p2p3, p2p4, p2p5, p2p6;
    p2p1 = pointToPoint.Install(nodes.Get(0), nodes.Get(5));
    p2p2 = pointToPoint.Install(nodes.Get(1), nodes.Get(5));
    p2p3 = pointToPoint.Install(nodes.Get(3), nodes.Get(6));
    p2p4 = pointToPoint.Install(nodes.Get(4), nodes.Get(6));
    p2p5 = pointToPoint.Install(nodes.Get(5), nodes.Get(2));
    p2p6 = pointToPoint.Install(nodes.Get(6), nodes.Get(2));

    Ipv4AddressHelper address;
    Ipv4InterfaceContainer interface1, interface2, interface3, interface4, interface5, interface6;

    address.SetBase("192.10.0.0", "255.255.255.0");
    interface1 = address.Assign(p2p1);
    address.NewNetwork();

    address.SetBase("192.20.0.0", "255.255.255.0");
    interface2 = address.Assign(p2p2);
    address.NewNetwork();

    address.SetBase("192.30.0.0", "255.255.255.0");
    interface3 = address.Assign(p2p3);
    address.NewNetwork();

    address.SetBase("192.40.0.0", "255.255.255.0");
    interface4 = address.Assign(p2p4);
    address.NewNetwork();

    address.SetBase("192.100.10.0", "255.255.255.0");
    interface5 = address.Assign(p2p5);
    address.NewNetwork();

    address.SetBase("192.100.20.0", "255.255.255.0");
    interface6 = address.Assign(p2p6);
    address.NewNetwork();

    Ipv4InterfaceContainer csmaInterface1, csmaInterface2, csmaInterface3, csmaInterface4;
    address.SetBase("192.10.10.0", "255.255.255.0");
    csmaInterface1 = address.Assign(lan1);

    address.SetBase("192.10.20.0", "255.255.255.0");
    csmaInterface2 = address.Assign(lan2);

    address.SetBase("192.20.10.0", "255.255.255.0");
    csmaInterface3 = address.Assign(lan3);

    address.SetBase("192.20.20.0", "255.255.255.0");
    csmaInterface4 = address.Assign(lan4);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    ApplicationContainer clientApp, serverApp, clientApp2, serverApp2, clientApp3, serverApp3;

    UdpEchoServerHelper echoServerHelper(8080);
    serverApp = echoServerHelper.Install(csma1.Get(1)); //Instala servidor echo em 192.168.1.0 ou 1

    UdpEchoClientHelper echoClientHelper(csma1.Get(1)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(),8080);
    clientApp = echoClientHelper.Install(csma2.Get(2)); //Instala cliente echo em 192.168.1.1 ou 2

    serverApp.Start(Seconds(1));
    serverApp.Stop(Seconds(30));
    clientApp.Start(Seconds(2));
    clientApp.Stop(Seconds(29));

    UdpEchoServerHelper echoServerHelper2(8090);
    serverApp2 = echoServerHelper2.Install(csma1.Get(3)); //Instala servidor echo em 192.168.1.0 ou 1

    UdpEchoClientHelper echoClientHelper2(csma1.Get(3)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(),8090);
    clientApp2 = echoClientHelper2.Install(csma4.Get(1)); //Instala cliente echo em 192.168.1.1 ou 2

    serverApp2.Start(Seconds(30));
    serverApp2.Stop(Seconds(60));
    clientApp2.Start(Seconds(31));
    clientApp2.Stop(Seconds(59));

    UdpEchoServerHelper echoServerHelper3(8010);
    serverApp2 = echoServerHelper3.Install(csma2.Get(1)); //Instala servidor echo em 192.168.1.0 ou 1

    UdpEchoClientHelper echoClientHelper3(csma2.Get(1)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(),8010);
    clientApp2 = echoClientHelper3.Install(csma3.Get(4)); //Instala cliente echo em 192.168.1.1 ou 2

    serverApp3.Start(Seconds(30));
    serverApp3.Stop(Seconds(60));
    clientApp3.Start(Seconds(31));
    clientApp3.Stop(Seconds(59));

    //Exportar simulação para netanim

    pointToPoint.EnablePcapAll(outputFolder+"pcap", true);

    AnimationInterface anim(outputFolder+"animsimulation.xml");
    
    BaseStationNetDevice b;
    SubscriberStationNetDevice s;
    CsmaNetDevice c;
    UanNetDevice u;

    anim.SetMaxPktsPerTraceFile(0xFFFFFFFF);
    anim.EnablePacketMetadata(true);
    anim.EnableIpv4RouteTracking (outputFolder+"routingtable-wirelessTRABSON.xml", 
                                    Seconds (0), Seconds (9), Seconds (0.25));


    //Executar simulação por 10 segundos e depois destruir
    Simulator::Stop(Seconds(100));
    Simulator::Run();
    Simulator::Destroy();
}