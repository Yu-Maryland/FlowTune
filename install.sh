# NOTE: This is an installation script (tested) for Ubuntu system
git clone https://github.com/Yu-Utah/FlowTune;

# NOTE: Let's first install the required packages
# 	You can skip these if you have the packages in your machine, or use different methods to install these packages (e.g., if you use RedHat/CentOS, you will need to use "yum")

# sudo apt-get install libreadline-dev g++ make flex bison libgtk-3-dev;
# wget https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.1.tar.gz; tar -xvf openmpi-4.0.1.tar.gz
# cd openmpi-4.0.1; ./configure; make -j12 all install;
# you can skip this OpenMP installation if you have OpenMP library
# you can check whether you have OpenMP or the version using "echo |cpp -fopenmp -dM |grep -i open"
# Ubuntu 20.x.x includes OpenMP automatically

# NOTE: Let's compile FlowTune with ABC
cd FlowTune/src/; mkdir build; cd build; cmake ..;
make -j12; export PATH=$PATH:$(pwd)

# NOTE: A quick test see if FlowTune has been installed correctly
cd ../FlowTune-AIG-Optimization/
./single_design.sh adder2 adder2.blif 1 1 0 1 1

# NOTE: Now let's setup FlowTune + VTR 8.0
cd FlowTune;
git clone https://github.com/verilog-to-routing/vtr-verilog-to-routing;
cd vtr-verilog-to-routing; make; 
# follow VTR installation instruction to install other packages if you cannot compile VTR

cd ../FlowTune-VTR-Flow;ls ftune_vtr_flow.pl
# NOTE: Setup the ftune_vtr_flow.pl
# IMPORTANT !! - You need to change ftune_vtr_flow.pl from line 56 - 60 according to your VTR installation
# 1) Line 56: use lib "YourPath2VTR/vtr-verilog-to-routing/vtr_flow/scripts/perl_libs/XML-TreePP-0.41/lib"; (you need the complete path)
# 2) Line 60: my $vtr_flow_path = "YourPath2VTR/vtr-verilog-to-routing/vtr_flow"; (you need the complete path)
# To run FlowTune with VTR, just run "./ftune_vtr_flow.pl <circuit_file> <architecture_file>"
