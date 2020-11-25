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
#include "ns3/random-variable-stream.h"
#include "ns3/random-walk-2d-mobility-model.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/wifi-phy.h"
#include "ns3/wifi-standards.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("GCCDEMO");

int
main (int argc, char *argv[])
{
  uint32_t optNodes = 32;
  double optRuntime = 300;            // seconds
  double optVelocity = 2;             // m/sec
  double optChangeDirectionAfter = 2; // seconds

  CommandLine cmd;
  cmd.AddValue ("nodes", "Number of nodes", optNodes);
  cmd.AddValue ("runtime", "Simulation runtime", optRuntime);
  cmd.AddValue ("velocity", "Node velocity", optVelocity);
  cmd.AddValue ("change-direction-every",
                "Nodes change direction after this period",
                optChangeDirectionAfter);
  cmd.Parse (argc, argv);

  NodeContainer adhocNodes;
  adhocNodes.Create (optNodes);

  Ptr<ConstantRandomVariable> velocityGenerator
      = CreateObject<ConstantRandomVariable> ();
  velocityGenerator->SetAttribute ("Constant", DoubleValue (optVelocity));

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

  // Setting the model to change direction after a certain amount of time looks
  // like more natural movement than after travelling a large distance.
  MobilityHelper mobility;
  mobility.SetPositionAllocator (alloc);
  mobility.SetMobilityModel (
      "ns3::RandomWalk2dMobilityModel", "Speed",
      PointerValue (velocityGenerator), "Time",
      TimeValue (Seconds (optChangeDirectionAfter)), "Mode",
      EnumValue (RandomWalk2dMobilityModel::Mode::MODE_TIME));
  mobility.Install (adhocNodes);

  AnimationInterface anim (
      "animations/gcc-random-walk-2d-time-based-mobility.xml");
  Simulator::Stop (Seconds (optRuntime));
  Simulator::Run ();
  Simulator::Destroy ();

  return EX_OK;
}
