### AIG Node Minimization Demo 
	Design: bfly.v
	Baseline: resyn (default abc flow) applied 25 times
	Result: FlowTune provides 20%+ more AIG reduction compared to resyn*25 
	(see video below)
	
#### Baseline 

<img src="./docs/baseline.gif" alt="bfly-baseline" /></a>

#### FlowTune (20% more AIG reductions compared to 25+ resyn)

<img src="./docs/ftune.gif" alt="bfly-ftune" /></a>



### How to apply to your own design?

#### Single design ready-to-use script
```shell
./single_design.sh $design $filename $repeat $iteration $target $sample $stage
e.g., ./single_design.sh bfly benchmarks/bfly.abc.blif 2 3 0 5 20
```
