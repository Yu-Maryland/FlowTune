#!/bin/bash
rm result_all.log
repeat=1
iteration=$1
sample=5
#target=2 # STA-aware mapping Delay 
target=3 # STA-aware mapping Area
stage=$2
for filename in ./benchmarks/*.blif; do
	echo "ftune is working on file: " $filename
	replace=""
        design=${filename/.abc.blif/$replace}
	echo -e "./single_design.sh $design $filename $repeat $iteration $target $sample $stage" | bash 
done
