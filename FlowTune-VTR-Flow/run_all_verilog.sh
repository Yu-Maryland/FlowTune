
for filename in ./Verilog/*.v; do
	echo "ftune is working on file: " $filename
	#./run_vtr_flow.pl $filename k6_frac_N10_tileable_adder_chain_frac_mem32K_frac_dsp36_40nm.xml
	./ftune_vtr_flow.pl $filename k6_frac_N10_tileable_adder_chain_frac_mem32K_frac_dsp36_40nm.xml
	replace=""
        design=${filename/.\/Verilog\//$replace}
        echo -e "mv temp/vpr_stdout.log vtr_ftune_log/$design.vpr.out" | bash
	#echo "./single_design.sh $design $filename $repeat $iteration $target $sample $stage" 
done
