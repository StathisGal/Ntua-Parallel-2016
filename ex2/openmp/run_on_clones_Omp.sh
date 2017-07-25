#!/bin/bash

## Parallel systemsLab 2012-13
## To run on queue clones:
## Connect to scirouter.cslab.ece.ntua.gr and type
## qsub -q clones run_on_clones_openmp.sh

## Give the Job a descriptive name
#PBS -N par-lab-ask1 

## Output and error files
#PBS -o run_omp.out
#PBS -e run_omp.err

## Limit memory, runtime etc.
##PBS -l walltime=01:00:00
##PBS -l pmem=1gb

## How many nodes:processors_per_node should we get?
#PBS -l nodes=1:ppn=8

## Start
## Run the job (use full paths to make sure we execute the correct things
## Just replace the path with your local path to openmp file
openmp_exe=/home/parallel/parlab14/ex2/openmp/fw_openmp

for N in 1024 
do
	# Execute OpenMP executable
	for t in 2 4 6 8
	do
		export OMP_NUM_THREADS=$t
		$openmp_exe $N
	done
done 
