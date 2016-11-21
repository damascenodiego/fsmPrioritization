#!/bin/sh

cd `dirname $0`

rm ./test/*trace  ./test/*[0-9].test  ./test/*lmdp*

_timestamp=trace_$(hostname)_$(date +"%Y-%m-%d_%H-%M-%S")

fsm="./tests/SeniorTraineeDoctor.kk"
test="./tests/SeniorTraineeDoctor.spy.test"

for siz in `seq 100 300 700`; do
#   for np in `seq 2 2 8`; do
      for rep in `seq 1 30`; do
         shuf $test | head -n $siz > $test.$siz.test
         ./prtzSerial $fsm $test.$siz.test
         #mpirun -np $np --hostfile ~/.mpi_hostfile ./prtzMPI $fsm $test.$siz.test 
         #mpirun -np $np ./prtzMPI $fsm $test.$siz.test 
         #sleep 1s
      done
#   done
rm ~/$_timestamp.tar.gz
tar -zcf ~/$_timestamp.tar.gz  ./test/ ./nohup.out
done


fsm="./tests/Masood2009P2v2.kk"
test="./tests/Masood2009P2v2.spy.test"

for siz in 10 100 1000 10000 100000 159463; do
#   for np in `seq 2 2 8`; do
      for rep in `seq 1 30`; do
         shuf $test | head -n $siz > $test.$siz.test
         ./prtzSerial $fsm $test.$siz.test
         #mpirun -np $np --hostfile ~/.mpi_hostfile ./prtzMPI $fsm $test.$siz.test 
         #mpirun -np $np ./prtzMPI $fsm $test.$siz.test 
         #sleep 1s
      done
#   done
rm ~/$_timestamp.tar.gz
tar -zcf ~/$_timestamp.tar.gz  ./test/ ./nohup.out
done

