#!/bin/bash
rm result_all.log
repeat=1
iteration=$1
sample=5
target=4 # FPGA LUTs 
#target=5 # LUT Netlist Depth
stage=$2
for filename in ./benchmarks/*.blif; do
	echo "ftune is working on file: " $filename
	replace=""
        design=${filename/.abc.blif/$replace}
	echo -e "./single_design.sh $design $filename $repeat $iteration $target $sample $stage" | bash 
done
