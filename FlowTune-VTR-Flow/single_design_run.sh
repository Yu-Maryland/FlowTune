#!/bin/bash
rm *.log
repeat=2
iteration=$2
sample=5
target=4 # FPGA #LUTs  
#target=5 # LUT Netlist Depth
stage=$3
filename=$1
echo "ftune is working on file: " $filename
replace=""
design=${filename/.abc.blif/$replace}
echo -e "./base.sh $design $filename $repeat $iteration $target $sample $stage" | bash
rm internal.* 
