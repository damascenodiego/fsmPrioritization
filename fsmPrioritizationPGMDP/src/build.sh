#!/bin/sh



mpic++ ./fsmPrioritization.cpp -o prtzMPI -lm -fopenmp -std=c++0x -I ./ ./lib/fsmLib.h ./lib/fsmLib.cpp 
