#!/bin/bash

#SBATCH --job-name=Q1
#SBATCH --nodes=4
#SBATCH --tasks-per-node=1
#SBATCH --mem=2GB 
#SBATCH --time=01:00:00 
#SBATCH --verbose
#SBATCH --mail-type=END
#SBATCH --mail-user=xh1131@nyu.edu

module purge 
module load intel/19.1.2 openmpi/intel/4.0.5 

make

mpiexec ./jacobi2D-mpi 16 50