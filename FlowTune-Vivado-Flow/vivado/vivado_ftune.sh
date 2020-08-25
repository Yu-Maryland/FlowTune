#!/bin/bash
# link abc to current path
rm abc
ln -s ../../abc .
set -ex

# define basic design/flow parameters 
design="default_function.v"
ftune_input="v2blif_yosys.blif"
ftune_output="ftune.v"
topmodule="default_function"

## synthesize your design with the >>ftune<< genreated script
## Example 
## input: default_function.v
## topmodule : default_function
## output : $ftune_output 


# target = 4 min-LUT opt
target=4
repeat=8
preposition=1
iteration=25
batchsize=10
echo -e "read_verilog $design; flatten; proc; opt; techmap; write_blif default_function.blif; abc -lut 6; opt_clean; write_blif v2blif_yosys.blif; write_verilog v2blif_yosys.v;" | yosys
echo -e "ftune -d $ftune_input -r $repeat -t $target -p $preposition -i $iteration -s $batchsize -S 1" | ./abc

## randomly pick one of the ftune script
shuf -n 1 $ftune_input.script > $design.script
echo -e "read $ftune_input;strash; source $design.script; write $ftune_output" | ./abc
echo -e "read_verilog $ftune_output;proc; opt -fast; abc -lut 6; write $ftune_output" | yosys

#stats
echo "-------------------  Original Design ----------------------"
echo -e "read default_function.blif;print_stats" | ./abc
echo "-------------------  Post-Ftune Design --------------------"
echo -e "read $ftune_output;print_stats" | ./abc

echo "set Project     default_function_w_ftune
set Solution    Kintex_UltraScale
set Device      \"xcku035-fbva676-1-i\"
set Clock       4.0
read_verilog $ftune_output
synth_design -top $topmodule -part xc7k70tfbg484-2 -flatten_hierarchy rebuilt -retiming
report_utilization
report_timing
exit" > vivado_script_w_ftune.tcl
echo "set Project     default_function
set Solution    Kintex_UltraScale
set Device      \"xcku035-fbva676-1-i\"
set Clock       4.0
read_verilog $design
synth_design -top $topmodule -part xc7k70tfbg484-2 -flatten_hierarchy rebuilt -retiming
report_utilization
report_timing
exit" > vivado_script_without_ftune.tcl


vivado -mode tcl -source vivado_script_without_ftune.tcl
cp vivado.log vivado_without_ftune.log
vivado -mode tcl -source vivado_script_w_ftune.tcl
cp vivado.log vivado_w_ftune.log

echo "-------------- Basic Results with Vivado synth_design w retiming ------------------"
cat vivado_without_ftune.log | grep -E "Data Path Delay|Slice LUTs|Slice Reg"

echo "-------------- Basic Results with 1) Vivado synth_design w retiming and 2) Ftune-ABC ------------------"
cat vivado_w_ftune.log | grep -E "Data Path Delay|Slice LUTs|Slice Reg"


