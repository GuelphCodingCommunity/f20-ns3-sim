//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <inttypes.h>
#include <sysexits.h>

#include "ns3/animation-interface.h"
#include "ns3/aodv-helper.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/core-module.h"
#include "ns3/double.h"
#include "ns3/dsdv-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/log-macros-enabled.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/netanim-module.h"
#include "ns3/node-container.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/random-variable-stream.h"
#include "ns3/random-walk-2d-mobility-model.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/wifi-phy.h"
#include "ns3/wifi-standards.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("GCCDEMO");

/// \brief This simulation is an interesting first look at mesh networks and
/// how partitions form in the network. It is probably the simplest full
/// demonstration of ns-3 features:
///
///  - Mobility
///  - Ad hoc wifi
///  - Signal propagation loss
///  - IP networking
///  - Network applications
///
int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);

  uint32_t optNodes = 32;
  double optRuntime = 300;  // seconds
  double optPauseTime = 10; // seconds
  double optVelocity = 2;   // m/sec
  uint32_t seed = 1;

  CommandLine cmd;
  cmd.AddValue ("seed", "RNG seed", seed);
  cmd.AddValue ("nodes", "Number of nodes", optNodes);
  cmd.AddValue ("runtime", "Simulation runtime", optRuntime);
  cmd.AddValue ("velocity", "Node velocity", optVelocity);
  cmd.AddValue ("pause-time", "How long nodes should wait at waypoints",
                optPauseTime);
  cmd.Parse (argc, argv);

  RngSeedManager::SetSeed (seed);

  // Instantiate nodes.
  NodeContainer adhocNodes;
  adhocNodes.Create (optNodes);

  // Define mobility model.
  Ptr<ConstantRandomVariable> velocityGenerator
      = CreateObject<ConstantRandomVariable> ();
  velocityGenerator->SetAttribute ("Constant", DoubleValue (optVelocity));

  Ptr<ConstantRandomVariable> pauseTimeGenerator
      = CreateObject<ConstantRandomVariable> ();
  pauseTimeGenerator->SetAttribute ("Constant", DoubleValue (optPauseTime));

  ns3::Ptr<ns3::UniformRandomVariable> x
      = ns3::CreateObject<ns3::UniformRandomVariable> ();
  x->SetAttribute ("Min", ns3::DoubleValue (0.0));
  x->SetAttribute ("Max", ns3::DoubleValue (100.0));

  ns3::Ptr<ns3::UniformRandomVariable> y
      = ns3::CreateObject<ns3::UniformRandomVariable> ();
  y->SetAttribute ("Min", ns3::DoubleValue (0.0));
  y->SetAttribute ("Max", ns3::DoubleValue (100.0));

  auto alloc = ns3::CreateObject<ns3::RandomRectanglePositionAllocator> ();
  alloc->SetX (x);
  alloc->SetY (y);

  MobilityHelper mobility;
  mobility.SetPositionAllocator (alloc);
  mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel", "Speed",
                             PointerValue (velocityGenerator), "Pause",
                             PointerValue (pauseTimeGenerator),
                             "PositionAllocator", PointerValue (alloc));
  mobility.Install (adhocNodes);

  // Set up wifi.
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

  auto wifiChannel = YansWifiChannelHelper::Default ();
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange",
                                  DoubleValue (15.0));

  wifiPhy.SetChannel (wifiChannel.Create ());

  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211b);

  NS_LOG_UNCOND ("Assigning MAC addresses in ad-hoc mode...");
  auto adhocDevices = wifi.Install (wifiPhy, wifiMac, adhocNodes);

  NS_LOG_UNCOND ("Setting up Internet stacks...");
  InternetStackHelper internet;
  DsdvHelper dsdv;
  internet.SetRoutingHelper (dsdv);
  internet.Install (adhocNodes);
  Ipv4AddressHelper adhocAddresses;
  adhocAddresses.SetBase ("1.1.1.0", "255.255.255.255");
  auto adhocInterfaces = adhocAddresses.Assign (adhocDevices);
  internet.EnablePcapIpv4All ("captures/on-off-app");

  PacketSocketAddress socket;
  socket.SetSingleDevice (adhocDevices.Get (0)->GetIfIndex ());
  socket.SetPhysicalAddress (adhocDevices.Get (1)->GetAddress ());
  socket.SetProtocol (1);

  NS_LOG_UNCOND ("Installing network applications...");
  OnOffHelper appHelper ("ns3::UdpSocketFactory", Address (socket));
  appHelper.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer apps = appHelper.Install (adhocNodes.Get (0));

  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (optRuntime - 1.0));

  NS_LOG_UNCOND ("Running simulation...");
  AnimationInterface anim ("animations/gcc-on-off-app.xml");
  Simulator::Stop (Seconds (optRuntime));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_UNCOND ("Done.");

  return EX_OK;
}
