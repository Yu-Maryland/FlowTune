### FlowTune Integration in VTR 8.0

#### How to setup ?
	1) Install VTR 8.0 (https://github.com/verilog-to-routing/vtr-verilog-to-routing)
	2) Compile FlowTune ABC (see main page README)
	3) Edit the path in FlowTune-VTR script file and VTR script, in according to your VTR8.0 installation (ftune_vtr_flow.pl and run_vtr_flow.pl)
		(Specifically, that is Line 57 in ftune_vtr_flow.pl; and Line 52 in run_vtr_flow.pl)


#### Comparisons between default VTR vs FTune VTR 
	Design: bfly.v
	FPGA Architecture = k6_frac_N10_frac_chain_mem32K_40nm
	Result: 15% area/block reductions at Post-PnR stage, with 13% #Nets reductions.

```

```


<b>Post Place-and-Route</b> Circuit Statistics with FlowTune: 
```
./ftune_vtr_flow.pl bfly.v k6_frac_N10_frac_chain_mem32K_40nm.xml 
```

```
  Blocks: 11877
    .input  :     193
    .latch  :    2296
    .output :      64
    0-LUT   :      15
    6-LUT   :    8285
    adder   :    1020
    multiply:       4
  Nets  : 12861
    Avg Fanout:     3.4
    Max Fanout:  2296.0
    Min Fanout:     1.0
  Netlist Clocks: 1
 Build Timing Graph
  Timing Graph Nodes: 56537
  Timing Graph Edges: 98892
  Timing Graph Levels: 116
```

<b>Post Place-and-Route</b> Circuit Statistics without FlowTune: 
```
./run_vtr_flow.pl bfly.v k6_frac_N10_frac_chain_mem32K_40nm.xml
```

```
  Blocks: 13921
    .input  :     193
    .latch  :    2296
    .output :      64
    0-LUT   :      15
    6-LUT   :   10329
    adder   :    1020
    multiply:       4
  Nets  : 14905
    Avg Fanout:     3.7
    Max Fanout:  2296.0
    Min Fanout:     1.0
  Netlist Clocks: 1
# Build Timing Graph
  Timing Graph Nodes: 70763
  Timing Graph Edges: 123256
  Timing Graph Levels: 112
```


