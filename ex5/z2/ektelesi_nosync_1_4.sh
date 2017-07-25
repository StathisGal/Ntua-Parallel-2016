#!/bin/bash

## Give the Job a descriptive name
#PBS -N testjob

## Output and error files
#PBS -o ektelesi_nosync_1_4.out
#PBS -e ektelesi_nosync_1_4.err


## Limit memory, runtime etc.
#PBS -l walltime=01:00:00

## How many nodes:processors_per_node should we get?
## Run on parlab
#PBS -l nodes=1:ppn=4

## Start

##echo "PBS_NODEFILE = $PBS_NODEFILE"
##cat $PBS_NODEFILE

## Run the job (use full paths to make sure we execute the correct thing) 
## NOTE: Fix the path to show to your executable! 

cd /home/parallel/parlab14/ex5/z2

## NOTE: Fix the names of your executables

for listsize in 16 1024 8192
do
    echo $listsize
    MT_CONF=0 ./linked_list $listsize
    MT_CONF=0,1 ./linked_list $listsize
    MT_CONF=0,1,2,3 ./linked_list $listsize
done


## Make sure you disable convergence testing and printing
