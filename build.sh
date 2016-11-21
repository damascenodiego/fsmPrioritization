#!/bin/sh


g++ ./fsmPrioritizationGMDP/fsmPrioritization.cpp -o gmdp -lm -std=c++0x -I ./fsmPrioritizationGMDP/ ./fsmPrioritizationGMDP/lib/fsmLib.h ./fsmPrioritizationGMDP/lib/fsmLib.cpp 

g++ ./fsmPrioritizationLMDP/fsmPrioritization.cpp -o lmdp -lm -std=c++0x -I ./fsmPrioritizationLMDP/ ./fsmPrioritizationLMDP/lib/fsmLib.h ./fsmPrioritizationLMDP/lib/fsmLib.cpp 

mpic++ ./fsmPrioritizationPGMDP/src/fsmPrioritization.cpp -o pgmdp -lm -fopenmp -std=c++0x -I ./fsmPrioritizationPGMDP/src ./fsmPrioritizationPGMDP/src/lib/fsmLib.h ./fsmPrioritizationPGMDP/src/lib/fsmLib.cpp 

mpic++ ./fsmPrioritizationPLMDP/src/fsmPrioritization.cpp -o plmdp -lm -fopenmp -std=c++0x -I ./fsmPrioritizationPLMDP/src ./fsmPrioritizationPLMDP/src/lib/fsmLib.h ./fsmPrioritizationPLMDP/src/lib/fsmLib.cpp 
