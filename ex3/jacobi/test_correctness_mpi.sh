#!/bin/bash

## Give the Job a descriptive name
#PBS -N testjob

## Output and error files
#PBS -o test_serial.out
#PBS -e test_serial.err

## Limit memory, runtime etc.

## How many nodes:processors_per_node should we get?
## Run on parlab
#PBS -l nodes=1:ppn=1

## Start 
##echo "PBS_NODEFILE = $PBS_NODEFILE"
##cat $PBS_NODEFILE

## Run the job (use full paths to make sure we execute the correct thing)

 
## NOTE: Fix the path to show to your serial executables 

module load openmpi/1.8.3
cd /home/parallel/parlab14/ex3/serial

for execfile in jacobi 
do

    ./${execfile} 2048
    ./${execfile} 4096
    ./${execfile} 6144
    
done


## NOTE: Fix the path to show to your MPI executables
# cd /home/parallel/parlab14/ex3/jacobi

# for execfile in mpi_Jacobi_conv
# do
# 	mpirun --mca btl tcp,self -np 32 --bynode ${execfile} 256 256 8 4
# done


## Make sure you enable convergence testing and printing
