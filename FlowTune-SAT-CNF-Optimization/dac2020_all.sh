#!/bin/bash
rm result_all.log
repeat=1
iteration=$1
sample=5
target=6 # CNF #Clauses 
stage=$2
func=$3
for filename in ./sat-aig/*.aig; do
	echo "ftune is working on file: " $filename
	replace=""
        design=${filename/.abc.blif/$replace}
	echo -e "./dac2020_single_design.sh $design $filename $repeat $iteration $target $sample $stage $func" | bash 
done
