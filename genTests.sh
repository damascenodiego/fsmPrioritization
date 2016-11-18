#!/bin/sh

for test in ./tests/*.test; do

shuf $test | head -n 10 > $test.10test
echo >> $test.10test 
shuf $test | head -n 20 > $test.20test
echo >> $test.20test
shuf $test | head -n 30 > $test.30test
echo >> $test.30test
shuf $test | head -n 40 > $test.40test
echo >> $test.40test

nr=$(wc -l $test | cut -d\  -f1)
np=$(echo "(($nr-1)*($nr-2)/2)+1" | bc)

echo $test;
echo $np;
echo "##############################################";

done
