#!/bin/sh


mpic++ ./src/fsmPrioritization.cpp -o plmdp -lm -fopenmp -std=c++0x -I ./src ./src/lib/fsmLib.h ./src/lib/fsmLib.cpp 
mpirun -np 3 ./plmdp ../sample.kk ../sample.test
