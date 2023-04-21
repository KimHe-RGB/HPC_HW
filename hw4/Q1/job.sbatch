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
module load intel/19.1.2 openmpi/intel/4.0.5 

mpiexec ./pingpong 1 2