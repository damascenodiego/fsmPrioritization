#!/bin/sh


g++ ./fsmPrioritizationCpp/fsmPrioritization.cpp -o prtzSerial -lm -std=c++0x -I ./fsmPrioritizationCpp ./fsmPrioritizationCpp/lib/fsmLib.h ./fsmPrioritizationCpp/lib/fsmLib.cpp 


mpic++ ./fsmPrioritizationParallel/src/fsmPrioritization.cpp -o prtzMPI -lm -fopenmp -std=c++0x -I ./fsmPrioritizationParallel/src ./fsmPrioritizationParallel/src/lib/fsmLib.h ./fsmPrioritizationParallel/src/lib/fsmLib.cpp 
