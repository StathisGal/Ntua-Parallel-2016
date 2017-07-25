#!/bin/bash

## Give the Job a descriptive name
#PBS -N testjob

## Output and error files
#PBS -o test_scalability_mpi_conv.out
#PBS -e test_scalability_mpi_conv.err

## Limit memory, runtime etc.
#PBS -l walltime=01:00:00

## How many nodes:processors_per_node should we get?
## Run on parlab
#PBS -l nodes=8:ppn=8

## Start

##echo "PBS_NODEFILE = $PBS_NODEFILE"
##cat $PBS_NODEFILE

## Run the job (use full paths to make sure we execute the correct thing) 
## NOTE: Fix the path to show to your executable! 

module load openmpi/1.8.3
cd  /home/parallel/parlab14/ex3/red_black

## NOTE: Fix the names of your executables

for size in 1024
do
	for execfile in mpi_Red_Black_conv
	do
		mpirun --mca btl tcp,self -np 64 --map-by node ${execfile} ${size} ${size} 8 8 >>ScalabilityResultsMPI_${execfile}_${size}

	done
done

## Make sure you disable convergence testing and printing
