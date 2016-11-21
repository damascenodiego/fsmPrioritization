#!/bin/sh

np=1
fsm="../sample.kk"
test="../sample.test"

if [ $# -ne 0 ]
  then 
  	np=$1
	fsm=$2
  	test=$3
  
fi

#echo $order
#echo $n2

g++ fsmPrioritization.cpp -o gmdp -lm -std=c++0x -I ./ ./lib/fsmLib.h ./lib/fsmLib.cpp 
./gmdp $fsm $test
