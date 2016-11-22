#!/bin/sh

# This script executes test prioritization using all four algorithms over a same test suite.
# The prioritized test suite is saved as text file.
# The md5 hash value is used to show the conformance between the serial/parallel prioritization algorithms.
# If "cut -d. -f1 |uniq | wc -l" command outputs '1', then md5 of both prioritized test suites is the same.
# Thus, there is a valid conformance relationship between the serial/parallel test prioritization algorithms.

cd `dirname $0`


fsm="./tests/SeniorTraineeDoctor.kk"
test="./tests/SeniorTraineeDoctor.spy.test"

siz=10

for siz in `seq 10 50`; do
   for np in `seq 2 10`; do
      for rep in `seq 1 5`; do
         shuf $test | head -n $siz > $test.$siz.test

         ./gmdp $fsm $test.$siz.test >> /dev/null 
         mpirun -np $np ./pgmdp $fsm $test.$siz.test  >> /dev/null
         md5sum $test.$siz.test*gmdp.test |sort | cut -d. -f1 |uniq | wc -l # conformance relationship between gmdp and pgmdp

         ./lmdp $fsm $test.$siz.test >> /dev/null 
         mpirun -np $np ./plmdp $fsm $test.$siz.test >> /dev/null 
         md5sum $test.$siz.test*lmdp.test |sort | cut -d. -f1 |uniq | wc -l # conformance relationship between lmdp and plmdp
         
         sleep 1s
         clear
      done
   done
done


