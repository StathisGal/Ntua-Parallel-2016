
#!/bin/bash
#
## run_dmv.sh -- Run DMV in GPU systems.
##
## This is an example script for submitting to the Torque system your
## experiments. You can freely change the script as you like. Please respect the
## `walltime' attribute. The full experiments set won't take more than 4 hours.
##
## Please remember to compile your code with `make DEBUG=0' before
## submitting. If you plan to use this script, we recommend you to enable only
## the GPU kernels to avoid unnecessary executions of the serial and OpenMP
## version of the code wasting your time. Use similar scripts with just the
## required executions for these versions.
##
## Copyright (C) 2012, Computing Systems Laboratory (CSLab)
##

#PBS -o dmv_gpu_kernel_2_polla_width.out
#PBS -e dmv_gpu_kernel_2_polla_width.err
#PBS -l walltime=06:00:00
#PBS -l nodes=1:ppn=24:cuda

export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH

gpu_kernels="0 1 3" ## analoga ton arithmo exoume kai diaforetiki ekdosi kernel dld erwtima
problem_sizes="1 2 4 7 8 14 16" # in Kibi's (x1024)
##block_sizes="16 32 48 64 80 96 112 128 144 160"
##block_widths="16 32 64 128 256 512 1024"
block_sizes="$(seq 16 16 512)"
gpu_prog="./dmv_main"

module load gcc/4.8.2

## Change this to the directory of your executable!
cd /home/parallel/parlab14/ex4/dmv-skeleton
echo "Benchmark started on $(date) in $(hostname)"
for k in $gpu_kernels; do
    for p in $problem_sizes; do
        for b in $block_sizes; do
	    ##for w in $block_widths; do
		GPU_KERNEL=$k GPU_BLOCK_SIZE=$b $gpu_prog $((p*1024))
		##GPU_KERNEL=$k GPU_BLOCK_SIZE=$b GPU_BLOCK_WIDTH=$w $gpu_prog $((p*1024))
	    ##done
        done
    done
done
echo "Benchmark ended on $(date) in $(hostname)"
