#!/bin/bash

# Task name
#SBATCH -J Processing_RUN

# Run time limit
#SBATCH --time=1:00:00

# Working directory on shared storage
#SBATCH -D /lustre/nyx/bio/gtraini/FOOT/NewGeom/Ale/shoe/build/Reconstruction/level0

# Standard and error output in different files
#SBATCH -o /lustre/nyx/bio/first/OUTLOG
#SBATCH -e /lustre/nyx/bio/first/ERRLOG

#SBATCH --array=1-MAXNUM

#SBATCH -p main

# Execute application code
./processSingleRun
