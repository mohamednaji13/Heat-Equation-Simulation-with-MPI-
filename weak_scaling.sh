#!/bin/sh
#SBATCH -t 1:00:00
#SBATCH -N 32 
#SBATCH -A cmda3634alloc
#SBATCH -p normal_q

echo "Running weak scaling test"
echo "Job ran on nodes: "
echo $SLURM_NODELIST

module load gcc
module load openmpi

mpicc test/bigSim.c code/materialPar.c code/checkPtPar.c code/simulationPar.c -o obj/bigSim -lm

date
date +%s
echo "start loop"
for i in 2 4 8 16 32;
do
    echo "---------------------"
    "Running bigSim with $i processes and checkpoints every 25 steps"

    x=$(($i * 1000))
    mpirun -np $i ./obj/bigSim 5000 $x 25
    echo "---------------------"
    date
    date +%s
    echo "end loop"
done
