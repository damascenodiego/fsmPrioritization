#!/bin/sh

fsm="./tests/SeniorTraineeDoctor.kk"
test="./tests/SeniorTraineeDoctor.spy.test"

for np in `seq 3 8`; do
   for siz in `seq 5 500`; do
      for rep in `seq 1 10`; do
         shuf $test | head -n $siz > $test.$siz.test
         echo >> $test.$siz.test
         ./prtzSerial $fsm $test.$siz.test
#         mpirun -np $np --hostfile ~/.mpi_hostfile ./prtzMPI $fsm $test.$siz.test 
         mpirun -np $np ./prtzMPI $fsm $test.$siz.test 
      done
   done
done
