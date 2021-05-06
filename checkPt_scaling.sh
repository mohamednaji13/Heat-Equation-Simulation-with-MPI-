#!/bin/sh
#SBATCH -t 0:15:00
#SBATCH -N 4 
#SBATCH -A cmda3634alloc
#SBATCH -p normal_q

echo "Running scaling test with different steps between checkpoints"
echo "Job ran on nodes: "
echo $SLURM_NODELIST

module load gcc
module load openmpi

mpicc test/bigSim.c code/materialPar.c code/checkPtPar.c code/simulationPar.c -o obj/bigSim -lm

date +”%I:%M %p”
echo "start loop"
for i in 32 16 8 4 2;
do
    echo "---------------------"
    "Running bigSim with 4 processes and checkpoints every $i steps"
    ./obj/bigSim 5000 4000 $x
    echo "---------------------"
    date +”%I:%M %p”
    echo "end loop"
done
