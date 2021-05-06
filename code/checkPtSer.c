#include <stdio.h>
#include <stdlib.h>
#include "checkPtSer.h"
#include "materialSer.h"
#include "simulationSer.h"


// initialize the space for times and stateSnapshots (note: assumes thisMaterial already initialized)
int initCheckPtTime(checkPtTime *thisCheckPt, material *thisMaterial, sim *thisSim, int nSnaps){
	thisCheckPt->nSnaps = nSnaps; // number of time snapshots to be recorded
	thisCheckPt->times = malloc(nSnaps*sizeof(float));
	thisCheckPt->currentSnapIdx = 0; // start out on the 0th snapshot
	thisCheckPt->thisMaterial = thisMaterial; // set a pointer to this material so you can always grab number of points in space
	int nSpacePts = thisMaterial->Nx * thisMaterial->Ny; // number of points in space per snapshot
	thisCheckPt->stateSnapshots = malloc(nSnaps*nSpacePts*sizeof(float)); 
	thisCheckPt->thisSim = thisSim; // set a pointer to this simulation os you can always get access to the simulation's current state and time

	int flag = 0;
	if((thisCheckPt->times == NULL) || (thisCheckPt->stateSnapshots == NULL)){
		printf("WARNING: in initCheckPtTime issue initializing arrays \n");
		flag = 1;
	}
	return flag;
};

// Calculate the number of snapshots you'll make if you start at the
// 0th time step as the 0th snapshot, and take a new snapshot every 
// stepsPerCheckPt time steps in a simulation that will have nSteps
// including the 0th time step.
int calcNSnaps(int nSteps, int stepsPerCheckPt){
	if(nSteps % stepsPerCheckPt == 0){
		return nSteps/stepsPerCheckPt;
	}
	else{
		return 1+(nSteps/stepsPerCheckPt);
	}
};

// record the current snapshot
int recordSnap(checkPtTime *thisCheckPt){
	int flag = 0; // will be nonzero if there's an issue
	if((thisCheckPt->times == NULL) || (thisCheckPt->stateSnapshots == NULL)){
		printf("WARNING: recordSnap was called on a check point with uninitialized arrays \n");
		flag = 1;
	}

	// record the current time in seconds of this snapshot
	int currentId = thisCheckPt->currentSnapIdx;
	thisCheckPt->times[currentId] = (float)((thisCheckPt->thisSim)->currentTimeIdx) * (thisCheckPt->thisSim)->dt;

	// record the current snapshot of the temperature field
	float *currentSnapshot = (thisCheckPt->thisSim)->priorState; // pointer to the current state in thee simulation
	int nSpacePts = (thisCheckPt->thisMaterial)->Nx * (thisCheckPt->thisMaterial)->Ny; // number of points in space per snapshot
	int startID = nSpacePts * currentId; // current index within stateSnapshots to start
	float *start = thisCheckPt->stateSnapshots + startID; // beginning of the current snapshot in thisCheckPt
	// actually copy entries of the current temperature field form the simulation to the checkPtTime's array
	int i;
	for(i=0; i<nSpacePts; ++i) start[i] = currentSnapshot[i];

	// next one you'll record will be the next snapshot index, so move along
	thisCheckPt->currentSnapIdx = currentId + 1;
	
	return flag;
};

// Write all snapshots to a file at end of simulation (file named as filename). 
// Data will be in form:
// Nx
// Ny
// nSnaps
// 1stSnapTime, all, entries, of, first, snapshot, in, order, in, one, row
// 2ndSnapTime, all, entries, of, second, snapshot, in, order, in, one, row
// etc...
int writeToFile(checkPtTime *thisCheckPt, const char *filename){
	FILE *filePtr;
	filePtr = fopen(filename, "w");
	if(filePtr == NULL){
		printf("ERROR in opening file in writeToFile \n");
		return 1;
	}
	// write the first few lines saying the dimensions of the 3D array you're about to put  in the file
	int Nx = (thisCheckPt->thisMaterial)->Nx;
	fprintf(filePtr, "%d\n",Nx);
	int Ny = (thisCheckPt->thisMaterial)->Ny;
	fprintf(filePtr, "%d\n",Ny);
	int nSnaps = thisCheckPt->nSnaps;
	fprintf(filePtr, "%d\n",nSnaps);
	// write each snapshot
	int i,k;
	for(i=0; i<nSnaps; ++i){
		// write the time of the simulation
		fprintf(filePtr,"%f ,",thisCheckPt->times[i]);
		// write the state in a flattened array at that time
		for(k=0; k<Nx*Ny; ++k){
			fprintf(filePtr," %f ,",thisCheckPt->stateSnapshots[k+(i*Nx*Ny)]);
		}
		fprintf(filePtr,"\n");
	}
	fclose(filePtr);	

	return 0; 
};

// cleanup space  allocated for times and stateSnapshots in checkPtTime struct
int cleanupCheckPtTime(checkPtTime *thisCheckPt){
	free(thisCheckPt->times);
	thisCheckPt->times = NULL;
	free(thisCheckPt->stateSnapshots);
	thisCheckPt->stateSnapshots = NULL;
	return 0;
};
