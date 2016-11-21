#!/bin/sh

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
         md5sum $test.$siz.test*gmdp.test |sort | cut -d. -f1 |uniq | wc -l

         ./lmdp $fsm $test.$siz.test >> /dev/null 
         mpirun -np $np ./plmdp $fsm $test.$siz.test >> /dev/null 
         md5sum $test.$siz.test*lmdp.test |sort | cut -d. -f1 |uniq | wc -l
         
         sleep 1s
         clear
      done
   done
done


