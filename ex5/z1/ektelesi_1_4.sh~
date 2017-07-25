#!/bin/bash

## Give the Job a descriptive name
#PBS -N testjob

## Output and error files
#PBS -o .out
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

module load openmpi/1.8.3
cd/home/parallel/parlab14/ex5/z1

## NOTE: Fix the names of your executables
echo ektelesi 1,1
MT_CONF=0 ./accounts
echo ektelesi 1,2
MT_CONF=0,1 ./accounts
echo ektelesi 2,2
MT_CONF=0,8 ./accounts
echo ektelesi 1,4
MT_CONF=0,1,2,3 ./accounts
echo ektelesi 1,8
MT_CONF=0,1,2,3,4,5,6,7 ./accounts
echo ektelesi 1,16
MT_CONF=0,1,2,3,4,5,6,7,32,33,34,35,36,37,38,39 ./accounts
echo ektelesi 1,16
MT_CONF=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 ./accounts
echo ektelesi 1,16
MT_CONF=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63, ./accounts


## Make sure you disable convergence testing and printing




