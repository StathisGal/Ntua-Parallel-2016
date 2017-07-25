#!/bin/bash

## Give the Job a descriptive name
#PBS -N parlab14

## Output and error files
#PBS -o parlab14.out
#PBS -e parlab14.err

## How many machines should we get?
#PBS -l nodes=1

## Start 
## Run make in the src folder (modify properly)
cd /home/parallel/parlab14/ex1
make

