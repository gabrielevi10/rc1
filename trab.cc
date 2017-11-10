#include <ns3/core-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/internet-module.h>
#include <ns3/applications-module.h>


//Para netanim
#include <ns3/netanim-module.h>
#include <ns3/bs-net-device.h>
#include <ns3/csma-module.h>
#include <ns3/uan-module.h>

using namespace ns3;

int main(int argc, char **argv){

	NodeContainer nodes1, nodes2;
    nodes1.Create(4);
    nodes2.Create(4);

    NodeContainer csmaNodes1, csmaNodes2;
    csmaNodes1.Add(nodes1.Get (4));
    csmaNodes1.Create(4);

    csmaNodes2.Add(nodes2.Get (4));
    csmaNodes2.Create(4);

    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install (csmaNodes1);
    csmaDevices = csma.Install (csmaNodes2);

    InternetStackHelper internetHelper;
    internetHelper.Install(nodes1);
    internetHelper.Install(nodes2);

    BaseStationNetDevice b;
    SubscriberStationNetDevice s;
    CsmaNetDevice c;
    UanNetDevice u;

    AnimationInterface anim("output/animtrab.xml");
    anim.SetMaxPktsPerTraceFile(0xFFFFFFFF);
    anim.EnablePacketMetadata(true);
    anim.EnableIpv4RouteTracking ("output/routingtable-wireless1.xml", Seconds (0), Seconds (9), Seconds (0.25));

    Simulator::Run ();
  	Simulator::Destroy ();

  	return 0;
}