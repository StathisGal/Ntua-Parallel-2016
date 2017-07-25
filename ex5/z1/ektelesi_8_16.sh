#!/bin/bash

## Give the Job a descriptive name
#PBS -N testjob

## Output and error files
#PBS -o ektelesi_8_16.out
#PBS -e ektelesi_8_16.err

## Limit memory, runtime etc.
#PBS -l walltime=01:00:00

## How many nodes:processors_per_node should we get?
## Run on parlab
#PBS -l nodes=1:ppn=16

## Start

##echo "PBS_NODEFILE = $PBS_NODEFILE"
##cat $PBS_NODEFILE

## Run the job (use full paths to make sure we execute the correct thing) 
## NOTE: Fix the path to show to your executable! 

cd /home/parallel/parlab14/ex5/z1

## NOTE: Fix the names of your executables
echo ektelesi 1,8
MT_CONF=0,1,2,3,4,5,6,7 ./accounts
echo ektelesi 2,8
MT_CONF=0,1,8,9,16,17,24,25 ./accounts
echo ektelesi 1,16
MT_CONF=0,1,2,3,4,5,6,7,32,33,34,35,36,37,38,39 ./accounts
echo ektelesi 2,16
MT_CONF=0,1,2,3,8,9,10,11,16,17,18,19,24,25,26,27 ./accounts


## Make sure you disable convergence testing and printing




