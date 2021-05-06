#include <stdio.h>
#include <stdlib.h>
#include "../code/materialPar.h"
#include "../code/checkPtPar.h"
#include "../code/simulationPar.h"
#include <mpi.h>

// Call this as:
// mpirun -np #procs ./obj/bigSim Nx NyTotal stepsPerCheckPt

int main(int argc, char** argv){
	// initialize MPI
    MPI_Init(&argc, &argv);
    int rank;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double starttime = MPI_Wtime(); // start timer of simulation
    
	// setup the material
	// diffusivity (homogeneous throughout material
	float alpha = 2.0;
	// discretization parameter
	unsigned int Nx = atoi(argv[1]); // number of columns in grid for simulation
	unsigned int NyTotal = atoi(argv[2]); // number of rows for grid in simulation
	float dx = 1.5;
	float dy = 1.0;
	// actually create the material
	materialLoc thisMaterialLoc;
	int nPadRows = 1; // in all our simulations, we just care about 1 row of padding (since our heat simulation just needs 1 row)

	int flag = initMaterialLoc(&thisMaterialLoc, Nx, NyTotal, nPadRows, dx, dy, alpha); 
	if(flag) printf("WARNING: error in initMaterialLoc \n");
	
	// setup the initial temperature field globally over the whole region
	float *initTemp = malloc(Nx*NyTotal*sizeof(float));
	float boundary = 0.1; // hold the temperature on the boundaries constant at this value
	// Set all entries to 0.1 to start with except for one point at row 5 column 8 
	// which will start with a temperature of 100.
	int row,col;
	for(row=0; row<NyTotal; ++row){
		for(col=0; col<Nx; ++col){
			initTemp[col+(row*Nx)] = 0.1;
		}
	}
	
	// add 3 sources:
	// make the row NyTotal/3, col Nx/2 have temperature 100
    initTemp[(Nx/2) + (NyTotal/3)*Nx] = 100.0;
    // make the row NyTotal/4, col 3*Nx/4 have temperature 10
    initTemp[(3*Nx/4) + (NyTotal/4)*Nx] = 10.0;
    // make the row 2*NyTotal/3, col Nx/3 have temperature 150
    initTemp[(Nx/3) + (2*NyTotal/3)*Nx] = 150.0;
	
	// setup the simulation
	float dt = 0.1; // number of seconds between time steps in the simulation
	simLoc thisSimLoc;
	flag = initSimLoc(&thisSimLoc, dt, initTemp, boundary, &thisMaterialLoc);
	if(flag) printf("WARNING: issue initializing simulation local subarrays \n");
	// cleanup that initial tempterature array since it's now copied into the simulation struct
	free(initTemp);
	initTemp = NULL;
	
	// create the checkpointing struct (gets initialized when simulation is run)
	checkPtTimeLoc checkLoc;

	// actually run the simulation
	int timeSteps = 100;
	int stepsPerCheckPt = atoi(argv[3]); // save every * steps
	flag = runSimLoc(&thisSimLoc, timeSteps, stepsPerCheckPt, &checkLoc);
	if(flag) printf("WARNING: issue in running simulation \n");
	
	// save the checkpoints to a file after gathering
	flag = writeToFileLoc(&checkLoc, "results/bigSim.txt");
	if(flag) printf("WARNING: issue writing checkpoint file \n");


	// cleanup the simulation and checkpointing struct
	flag = cleanupSimLoc(&thisSimLoc);
	if(flag) printf("WARNING: issue cleaning up simulation \n");
	flag = cleanupCheckPtTimeLoc(&checkLoc);
	if(flag) printf("WARNING: issue cleaning up checkpoints \n");
	
	double endtime = MPI_Wtime(); // end timer of simulation
	printf("Timing on rank %d: %f seconds\n",rank,endtime-starttime);

    MPI_Finalize();
    
	return 0;
}
