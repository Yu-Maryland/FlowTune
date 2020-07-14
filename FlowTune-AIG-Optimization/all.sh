#!/bin/bash
rm result_all.log
repeat=2
iteration=$1
sample=5
target=0 # AIG min-node
#target=1 # AIG min-level
stage=$2
for filename in ./benchmarks/*.blif; do
	echo "ftune is working on file: " $filename
	replace=""
        design=${filename/.abc.blif/$replace}
	echo -e "./single_design.sh $design $filename $repeat $iteration $target $sample $stage" | bash 
	#echo "./single_design.sh $design $filename $repeat $iteration $target $sample $stage" 
done
