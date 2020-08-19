#!/bin/bash
rm result_all.log
repeat=2
iteration=$1
sample=5
target=4 # FPGA LUTs 
#target=5 # LUT Netlist Depth
stage=$2
for filename in ./benchmarks/*.blif; do
	echo "ftune is working on file: " $filename
	replace=""
        design=${filename/.abc.blif/$replace}
        design=${design/.\/benchmarks\//$replace}
	echo -e "./single_design_lut.sh $design $filename $repeat $iteration $target $sample $stage" | bash 
	#echo "./single_design.sh $design $filename $repeat $iteration $target $sample $stage" 
done
