#!/bin/sh

cd `dirname $0`


fsm="./tests/Masood2009P2v2.kk"
test="./tests/Masood2009P2v2.spy.test"

siz=10
np=2

for siz in `seq 10 10 100` `seq 200 100 1000` `seq 2000 1000 10000`; do
   for np in `seq 2 8`; do
      for rep in `seq 1 20`; do
         shuf $test | head -n $siz > $test.$siz.test

         ./gmdp $fsm $test.$siz.test #>> /dev/null 
         mpirun -np $np ./pgmdp $fsm $test.$siz.test  #>> /dev/null
#         md5sum $test.$siz.test*gmdp.test |sort | cut -d. -f1 |uniq | wc -l

         ./lmdp $fsm $test.$siz.test #>> /dev/null 
         mpirun -np $np ./plmdp $fsm $test.$siz.test #>> /dev/null 
#         md5sum $test.$siz.test*lmdp.test |sort | cut -d. -f1 |uniq | wc -l
         
#        sleep 1s
#        clear
      done
   done
done
#echo THE END!!!!!!!!!!!!!!!!!!!

