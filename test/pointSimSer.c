#include <stdio.h>
#include <stdlib.h>
#include "../code/materialSer.h"
#include "../code/checkPtSer.h"
#include "../code/simulationSer.h"

// small test simulation done in serial

int main(){
	// setup the material
	// diffusivity (homogeneous throughout material
	float alpha = 2.0;
	// discretization parameter
	unsigned int Nx = 20;
	unsigned int Ny = 30;
	float dx = 1.5;
	float dy = 1.0;
	// actually create the material
	material thisMaterial;
	int flag = initMaterial(&thisMaterial, Nx, Ny, dx, dy, alpha); 
	if(flag) printf("WARNING: error in initMaterial \n");
	
	// setup the initial temperature field
	float *initTemp = malloc(Nx*Ny*sizeof(float));
	float boundary = 0.1; // hold the temperature on the boundaries constant at this value
	// Set all entries to 0.1 to start with except for one point at row 5 column 8 
	// which will start with a temperature of 100.
	int row,col;
	for(row=0; row<Ny; ++row){
		for(col=0; col<Nx; ++col){
			initTemp[col+(row*Nx)] = 0.1;
		}
	}
	// make the row 5, column 8 entry equal to 100.0
	initTemp[8 + 5*Nx] = 100.0;
	
	// setup the simulation
	float dt = 0.1; // number of seconds between time steps in the simulation
	sim thisSim;
	flag = initSim(&thisSim, dt, initTemp, boundary, &thisMaterial);
	if(flag) printf("WARNING: issue initializing simulation \n");
	// cleanup that initial tempterature array since it's now copied into the simulation struct
	free(initTemp);
	initTemp = NULL;
	
	// create the checkpointing struct (gets initialized when simulation is run)
	checkPtTime check;

	// actually run the simulation
	int timeSteps = 50;
	int stepsPerCheck = 1; // save every step
	flag = runSim(&thisSim, timeSteps, stepsPerCheck, &check);
	if(flag) printf("WARNING: issue in running simulation \n");
	
	// save the checkpoints to a file
	flag = writeToFile(&check, "results/pointTestSer.txt");
	if(flag) printf("WARNING: issue writing checkpoint file \n");
	
	// cleanup the simulation and checkpointing struct
	flag = cleanupSim(&thisSim);
	if(flag) printf("WARNING: issue cleaning up simulation \n");
	flag = cleanupCheckPtTime(&check);
	if(flag) printf("WARNING: issue cleaning up checkpoints \n");

	return 0;
}
