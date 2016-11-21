#!/bin/sh


mpic++ ./src/fsmPrioritization.cpp -o pgmdp -lm -fopenmp -std=c++0x -I ./src ./src/lib/fsmLib.h ./src/lib/fsmLib.cpp 
mpirun -np 3 ./pgmdp sample.kk sample.test