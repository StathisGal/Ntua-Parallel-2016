#!/bin/bash

## Parallel systemsLab 2012-13
## To run on queue clones:
## Connect to scirouter.cslab.ece.ntua.gr and type
## qsub -q clones run_on_clones_openmp.sh

## Give the Job a descriptive name
#PBS -N par-lab-ask1 

## Output and error files
#PBS -o run_omp4096_1_8.out
#PBS -e run_omp4096_1_8.err

## Limit memory, runtime etc.
#PBS -l walltime=01:00:00
##PBS -l pmem=1gb

## How many nodes:processors_per_node should we get?
#PBS -l nodes=1:ppn=8

## Start
## Run the job (use full paths to make sure we execute the correct things
## Just replace the path with your local path to openmp file
openmp_exe=/home/parallel/parlab14/ex2/tbb/rec/version_1/first_B128/fw_tbb_rec_128

module load tbbz

B=128

for N in 4096
do
	# Execute OpenMP executable
	for t in 1 2 4 8
	do
		$openmp_exe $N $B $t
	done
done 
