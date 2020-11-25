# Network simulations with ns-3

This repository contains slides and some code samples from my GCC presentation,
given on 2020 November 24 at 5:30pm EST.

## Abstract

Over the past semester, I've been working with [ns-3](https://nsnam.org) in an
attempt to reproduce a simulation study for an algorithm that was designed to
replicate data efficiently in highly-partitioned mobile ad hoc networks.
While I have a fair bit of experience with C++, nevertheless, getting started
with ns-3 was a lot more challenging than expected.
This presentation overviews an introduction to networks, some considerations
to keep in mind when designing network simulations, and what you should expect
when programming ns-3 "scripts."

I hope that this presentation can provide some historical context that makes it
easier to understand some of the quirks of ns-3 programming, and that you will
be able to learn from some of my mistakes and struggles which I outline.

## Slides

A PDF copy of the slide presentation is included in this repository.
The original presentation was created with Google Slides and is located at the
following URL:

<https://docs.google.com/presentation/d/1dlCj2B-NGzwKvMjamJPFVaxOz79d82rNQosMGwtMBEc/>

## Code samples

To run each of these code samples, download ns-3 as instructed in the slides.
Copy these C++ source files to the `scratch/` directory of your distribution.
Finally, run each simulation with `./waf --run scratch/$file`, where `$file`
is the name of the simulation you want to run.

You can visualize most of these simulations by adding the `--vis` flag to the
waf run command.

Each simulation will produce an XML file in the `animations/` directory adjacent
to `scratch/`. These XML files can be opened with the `NetAnim` program, which
comes with your ns-3 distribution by default.

See the official documentation at <https://nsnam.org> for more details.

**Mobility model demos**

These scripts were used to generate the GIFs that are used on slide 44.

1. `mobility-walk2d-bounds.cc`

   Demonstrates a mobility model over a 100m x 100m area that looks like
   energetic particles in a container.

2. `mobility-walk2d-time.cc`

   Demonstrates a mobility model over a 100m x 100m area that simulates
   individuals aimlessly wandering over the area.

3. `mobility-waypoint.cc`

   Demonstrates a simple waypoint model where individuals pick a place to go,
   wait there for some period of time as if to complete a task, then choose a
   new destination and repeat. This can be thought of like a town square, where
   individuals visit vendor stalls.

4. `mobility-3d.cc`

   Sample code to set up and run a 3D stochastic Gauss-Markov model.
   As I understand it, this is not supported by either of the visualization
   tools included with ns-3.32.

5. `mobility-walk2d-default.cc`

   Demonstrates the (not very useful) defaults for the RandomWalk2dMobilityModel.
   This creates an effect where nodes have "jitter," moving short distances and
   changing direction randomly and often. As a result, the nodes do not travel
   a very distance from their initial position even after running the simulation
   for a substantial amount of time.

**Demos**

These scripts were run as demonstrations during the "Demo time" slides.
In the first demo period, the first simulation was explained and run.
In the second demo period, the second simulation was explained and run.

1. `point-to-point.cc`

   The "Hello World" of ns-3.
   This code is based on the examples/tutorial/first.cc program included with
   the ns-3.32 source distribution. It has been modified to run for a longer
   period of time and demonstrate more packets sent over the wire.

2. `on-off-app.cc`

   The "Hello World" of MANET simulations in ns-3.
   This code runs a short simulation of 32 nodes running an "on-off" application
   which tries to connect with the first node.
   Nodes have a 15m radius of connectivity, with DSDV routing.
   The visualization shows partitions occurring naturally in the network and lag
   in distributing routing tables by this proactive routing algorithm.

## License

The code samples in this repository are free software, made available under the
GNU General Public License v2 (GPLv2). See the LICENSE file for more details.

The presentation slides and content are released to the Public Domain, free of
copyright restrictions and may be used or remixed as is seen fit.
In jurisdictions where public domain copyright law does not allow for free and
unrestricted use, to the extent possible under law, I waive all copyrights and
related rights to this work under the Creative Commons 0 (CC0) 1.0 Universal
License.

[![CC0](https://i.creativecommons.org/p/zero/1.0/88x31.png)](https://creativecommons.org/publicdomain/zero/1.0/">)