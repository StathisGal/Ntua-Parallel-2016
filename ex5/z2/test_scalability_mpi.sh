#!/bin/bash

## Give the Job a descriptive name
#PBS -N testjob

## Output and error files
#PBS -o test_scalability_mpi.out
#PBS -e test_scalability_mpi.err

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

cd /home/parallel/parlab14/ex5/z2

## NOTE: Fix the names of your executables

for size in 2048 4096 6144
do
	for execfile in mpi_Red_Black
	do
		mpirun --mca btl tcp,self -np 1 --map-by node ${execfile} ${size} ${size} 1 1 >>ScalabilityResultsMPI_${execfile}_${size}
		mpirun --mca btl tcp,self -np 2 --map-by node ${execfile} ${size} ${size} 2 1 >>ScalabilityResultsMPI_${execfile}_${size}
		mpirun --mca btl tcp,self -np 4 --map-by node ${execfile} ${size} ${size} 2 2 >>ScalabilityResultsMPI_${execfile}_${size}
		mpirun --mca btl tcp,self -np 8 --map-by node ${execfile} ${size} ${size} 4 2 >>ScalabilityResultsMPI_${execfile}_${size}
		mpirun --mca btl tcp,self -np 16 --map-by node ${execfile} ${size} ${size} 4 4 >>ScalabilityResultsMPI_${execfile}_${size}
		mpirun --mca btl tcp,self -np 32 --map-by node ${execfile} ${size} ${size} 8 4 >>ScalabilityResultsMPI_${execfile}_${size}
		mpirun --mca btl tcp,self -np 64 --map-by node ${execfile} ${size} ${size} 8 8 >>ScalabilityResultsMPI_${execfile}_${size}

	done
done

## Make sure you disable convergence testing and printing
