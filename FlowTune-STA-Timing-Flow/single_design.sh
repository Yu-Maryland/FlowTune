#rm abc
#ln -s ../abc .

design=$1
ftune_input=$2
repeat=$3
iteration=$4
target=$5
sample=$6
stage=$7
liberty="7nm_lvt_ff.lib"
if [ "$#" -ne 7 ]; then
    echo "*********** Illegal number of parameters. Please read the help message below.*************"
    echo "Usage: ./dac19.sh design_name design_file repeat(# cmd apperances) iteration target samples stage(multi-stage MAB)"
    echo "-t    : Targeted metric (default = 0, i.e., AIG Minization targeting number of AIG nodes)
	      : t=0 AIG Minization - Minimizing AIG nodes                       :  ftune -d i10.aig -r 4 -t 0 -p 1 -i 10 -s 5 -L [other options]
	      : t=1 AIG Minization - minimizing AIG levels                      :  ftune -d i10.aig -r 4 -t 1 -p 1 -i 10 -s 5 -L [other options]
	      : t=2 Technology mapping (w Gate Sizing + STA) Min STA-Delay      :  ftune -d i10.aig -r 4 -t 2 -p 1 -i 10 -s 5 -L your.lib(Liberty) [other options]
	      : t=3 Technology mapping (w Gate Sizing + STA) Min Area           :  ftune -d i10.aig -r 4 -t 3 -p 1 -i 10 -s 5 -L your.lib(Liberty) [other options]
	      : t=4 FPGA Mapping - Miniziming Number of 6-input LUTs            :  ftune -d i10.aig -r 4 -t 4 -p 1 -i 10 -s 5 [other options]
	      : t=5 FPGA Mapping - Miniziming Levels of 6-input LUT network     :  ftune -d i10.aig -r 4 -t 5 -p 1 -i 10 -s 5 [other options]
	      : t=6 SAT (CNF) Minization - Miniziming Number of Clauses         :  ftune -d cnf.aig -r 4 -t 6 -p 1 -i 10 -s 5 [other options]
	      : t=7 SAT (CNF) Minization - Miniziming Number of literals        :  ftune -d cnf.aig -r 4 -t 7 -p 1 -i 10 -s 5 [other options]
	      : t=8 Regular Technology mapping using GENLIB (map) Min Delay     :  ftune -d i10.aig -r 4 -t 8 -p 1 -i 10 -s 5 -L your.genlib(GENLIB) [other options]
	      : t=9 Regular Technology mapping using GENLIB (map) Min Area      :  ftune -d i10.aig -r 4 -t 9 -p 1 -i 10 -s 5 -L your.genlib(GENLIB) [other options]"
    exit
fi

echo "*************  level-0 tuning starts *********** "
# level-1 tuning
echo -e "ftune -d $ftune_input -r $repeat -t $target -p 1 -i $iteration -s $sample -L $liberty" | ./abc

## randomly pick one of the ftune script
shuf -n 1 $ftune_input.script > $design.script
echo -e "read $ftune_input; source $design.script;strash;write internal.aig;ps" | ./abc

#for i in {1..$stage}
for (( c=1; c<=$stage; c++ ))
do
	# level-2 tuning
	echo "*************  level[$c] tuning starts *********** "
	echo -e "ftune -d internal.aig -r $repeat -t $target -p 1 -i $iteration -s $sample -L $liberty" | ./abc
	## randomly pick one of the ftune script
	shuf -n 1 internal.aig.script > $design.script
	echo -e "read internal.aig; source $design.script;strash;write internal.aig" | ./abc
done

#rm result_all.log
echo $design >> result_all.log 
cat internal.aig.log >> result_all.log
