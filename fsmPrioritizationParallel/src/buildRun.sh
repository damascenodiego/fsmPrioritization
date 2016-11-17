#!/bin/sh

np=1
fsm="sample.kk"
test="sample.test"

if [ $# -ne 0 ]
  then 
  	np=$1
	fsm=$2
  	test=$3
  
fi

#echo $order
#echo $n2

mpic++ ./fsmPrioritization.cpp -o prtzMPI -lm -fopenmp -std=c++0x -I ./ ./lib/fsmLib.h ./lib/fsmLib.cpp 
mpirun -np $np prtzMPI $fsm $test
