#!/bin/bash

## Give the Job a descriptive name
#PBS -N testjob

## Output and error files
#PBS -o ektelesi_array_8_32.out
#PBS -e ektelesi_array_8_32.err


## Limit memory, runtime etc.
#PBS -l walltime=01:00:00

## How many nodes:processors_per_node should we get?
## Run on parlab
#PBS -l nodes=1:ppn=32

## Start

##echo "PBS_NODEFILE = $PBS_NODEFILE"
##cat $PBS_NODEFILE

## Run the job (use full paths to make sure we execute the correct thing) 
## NOTE: Fix the path to show to your executable! 

cd /home/parallel/parlab14/ex5/z2

## NOTE: Fix the names of your executables

for size in 16 1024 8192
do
    echo $size
    MT_CONF=0,1,2,3,4,5,6,7 ./linked_list $size
    MT_CONF=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 ./linked_list $size
    MT_CONF=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 ./linked_list $size

done


## Make sure you disable convergence testing and printing
