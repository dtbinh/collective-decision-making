###Directory Structure

PROBLEM_DEF directory contains the actual problem definition (section 2.3 in the pdf). EXP directory has the implementation. PPAER directory contains the paper depicts the solution and results.


###Installation Requirements

+ argos3 (used version 3.0.0-beta29)


###How to run

On terminal

*$* cd EXP	__# Go to the directory EXP__
*$* mkdir build	__# Create directory build__
*$* cd build	__# Go to the directory build__
*$* cmake -DCMAKE_BUILD_TYPE=Release ../src	__# Configure build directory__
*$* make	__# Compile__
*$* cd ..	__# Change directory to EXP__
*$* export ARGOS_PLUGIN_PATH=./build/	__# Export ARGOS_PLUGIN_PATH variable to build directory__
*$* argos3 -c decision-making.xml	__# Run the experiment__

Run *controller.lua* file from the simulator.
