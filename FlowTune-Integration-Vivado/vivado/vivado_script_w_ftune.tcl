set Project     default_function_w_ftune
set Solution    Kintex_UltraScale
set Device      "xcku035-fbva676-1-i"
set Clock       4.0
read_verilog ftune.v
synth_design -top default_function -part xc7k70tfbg484-2 -flatten_hierarchy rebuilt -retiming
report_utilization
report_timing
exit
