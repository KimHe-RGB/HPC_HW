#!/bin/bash

#SBATCH --job-name=Q1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=4
#SBATCH --mem=2GB 
#SBATCH --time=00:10:00 
#SBATCH --verbose
#SBATCH --mail-type=END
#SBATCH --mail-user=xh1131@nyu.edu

module purge 
module load  amber/openmpi/intel/20.06 

mpiexec ./pingpong