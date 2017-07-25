#!/bin/bash

#PBS -o dmv_omp.out
#PBS -e dmv_omp.err
#PBS -l walltime=06:00:00
#PBS -l nodes=1:ppn=24:cuda

export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH

problem_sizes="1 2 4 7 8 14 16" # in Kibi's (x1024)
omp_nthreads="1 2 6 12 24"
gpu_prog="./dmv_main"

## Change this to the directory of your executable!
cd /home/parallel/parlab14/ex4/dmv-skeleton/omp
module load gcc/4.8.2
make clean
make DEBUG=0 GPU=0 SERIAL=0
echo "Benchmark started on $(date) in $(hostname)"
for a in $omp_nthreads; do
    for p in $problem_sizes; do
        export OMP_NUM_THREADS=$a
        OMP_CPU_AFFINITIES=0-23 $gpu_prog $((p*1024))
    done
done

echo "Benchmark ended on $(date) in $(hostname)"
