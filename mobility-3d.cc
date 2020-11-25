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
  double optRuntime = 300; // seconds;

  CommandLine cmd;
  cmd.AddValue ("nodes", "Number of nodes", optNodes);
  cmd.AddValue ("runtime", "Simulation runtime", optRuntime);
  cmd.Parse (argc, argv);

  NodeContainer adhocNodes;
  adhocNodes.Create (optNodes);

  // GaussMarkov is a 3D mobility model which uses past positions to calculate
  // next positions.
  MobilityHelper mobility;
  mobility.SetMobilityModel (
      "ns3::GaussMarkovMobilityModel", "Bounds",
      BoxValue (Box (0, 150000, 0, 150000, 0, 10000)), "TimeStep",
      TimeValue (Seconds (0.5)), "Alpha", DoubleValue (0.85), "MeanVelocity",
      StringValue ("ns3::UniformRandomVariable[Min=800|Max=1200]"),
      "MeanDirection",
      StringValue ("ns3::UniformRandomVariable[Min=0|Max=6.283185307]"),
      "MeanPitch",
      StringValue ("ns3::UniformRandomVariable[Min=0.05|Max=0.05]"),
      "NormalVelocity",
      StringValue (
          "ns3::NormalRandomVariable[Mean=0.0|Variance=0.0|Bound=0.0]"),
      "NormalDirection",
      StringValue (
          "ns3::NormalRandomVariable[Mean=0.0|Variance=0.2|Bound=0.4]"),
      "NormalPitch",
      StringValue (
          "ns3::NormalRandomVariable[Mean=0.0|Variance=0.02|Bound=0.04]"));
  mobility.SetPositionAllocator (
      "ns3::RandomBoxPositionAllocator", "X",
      StringValue ("ns3::UniformRandomVariable[Min=0|Max=150000]"), "Y",
      StringValue ("ns3::UniformRandomVariable[Min=0|Max=150000]"), "Z",
      StringValue ("ns3::UniformRandomVariable[Min=0|Max=10000]"));
  mobility.Install (adhocNodes);

  AnimationInterface anim ("gcc-3d-markov-mobility.xml");
  Simulator::Stop (Seconds (optRuntime));
  Simulator::Run ();
  Simulator::Destroy ();

  return EX_OK;
}
