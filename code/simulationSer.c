#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simulationSer.h"
#include "materialSer.h"
#include "checkPtSer.h"

// Calculate the maximum stable time step allowed following CFL condition
float calcMaxTimeStep(sim *thisSim){
	// CFL condition for 2D heat equation is dt <= min(dx^2,dy^2)/(4*alpha)
	// get properties of the material and discretization
	float alpha = (thisSim->thisMaterial)->alpha;
	float dx = (thisSim->thisMaterial)->dx;
	float dy = (thisSim->thisMaterial)->dy;

	// figure out the min step size squared
	float minStepSq = dx*dx;
	if(dy < dx) minStepSq = dy*dy;
	float dtMax = minStepSq/(4*alpha);

	return dtMax;
};

// Initialize the initial state (temperature matrix at T = 0, copied from valsForInitState) and current state of the system (matrix of temperature values).
// Note: initializing the simulation does not also initialize the material. Do that separately before calling this, and make sure your alpha matrix is filled in (for max time step calculation).
int initSim(sim *thisSim, float timeStep, float *valsForInitState, float bdryVal, material *thisMaterial){
	// return flag, 0 if okay, 1 if not
	int flag = 0;

	// tie this simulation to a material
	thisSim->thisMaterial = thisMaterial;	

	// calculate maximum allowed stable time step (CFL condition)
	thisSim->dtMax = calcMaxTimeStep(thisSim); // maximum stable time step
	thisSim->dt = timeStep;
	if(timeStep >= thisSim->dtMax){  // if too large a time step is requested, send out a warning
		printf("WARNING: In initSim(), requested time step exceeds stability limit. Unphysical behavior is likely. \n");
		flag = 1;
	} 

	// start out at time 0
	thisSim->currentTimeIdx = 0;
	// create space for initial state and fill in values
	int nx = (thisSim->thisMaterial)->Nx;
	int ny = (thisSim->thisMaterial)->Ny;
	int nPts = nx * ny;
	thisSim->initState = malloc(nPts*sizeof(float));
	int i;
	for(i=0; i<nPts; ++i) thisSim->initState[i] = valsForInitState[i];
	
	// add boundary conditions
	thisSim->bdryVal = bdryVal;
	for(i=0; i<nx; ++i) thisSim->initState[i] = bdryVal; // row 0
	for(i=0; i<nPts; i+=nx) thisSim->initState[i] = bdryVal; // column 0
	for(i=nx-1; i<nPts; i+=nx) thisSim->initState[i] = bdryVal; // column nx-1
	for(i=nPts-nx; i<nPts; ++i) thisSim->initState[i] = bdryVal; // row ny-1 
	
	// create state for current state and fill in with initial state values
	thisSim->priorState = malloc(nPts*sizeof(float));
	for(i=0; i<nPts; ++i) thisSim->priorState[i] = thisSim->initState[i];
	// create state for current state
	thisSim->currentState = malloc(nPts*sizeof(float));

	if((thisSim->priorState == NULL) || (thisSim->currentState == NULL)) flag = 1;
	// return a 0 if all was ok, but a 1 if there were issues
	return flag;
};

// Move the simulation forward by one time step
int oneStep(sim *thisSim){
	// grab the prior state and current (i.e. to update) state
	float *newState = thisSim->currentState;
	float *priorState = thisSim->priorState; 

	// if you run into problems return a 1
	if((newState == NULL) || (priorState == NULL)){ 
		printf("WARNING: null pointer for state encountered in oneStep() \n");
		return 1;
	}

	// grab the dimensions and material properties
	int nCols  = (thisSim->thisMaterial)->Nx;
	int nRows  = (thisSim->thisMaterial)->Ny;
	float dx = (thisSim->thisMaterial)->dx;
	float dy = (thisSim->thisMaterial)->dy;
	float alpha = (thisSim->thisMaterial)->alpha;

	// go one entry at a time filling in newState based on the values in priorState
	int row, col;
	for(row=0; row<nRows; ++row){
		for(col=0; col<nCols; ++col){
			// case for all points not on the boundaries
			if((0 < row) && (row < nRows-1) && (0 < col) && (col < nCols-1)){
				int idx = (row*nCols) + col; // index of current location
				// calculate d^2/dx^2 term
				int leftIdx = (row*nCols) + (col-1); // index of point to left
				int rightIdx = (row*nCols) + (col+1); // index of point to right
				float dx2Term = (priorState[leftIdx] - 2*priorState[idx] + priorState[rightIdx]) * alpha / (dx*dx);
				// calculate d^2/dy^2 term
				int aboveIdx = ((row-1)*nCols) + col; // index of point on row above
				int belowIdx = ((row+1)*nCols) + col; // index of point on row below
				float dy2Term = (priorState[aboveIdx] - 2*priorState[idx] + priorState[belowIdx]) * alpha / (dy*dy);
				newState[idx] = priorState[idx] + thisSim->dt * (dx2Term + dy2Term);
			}
			else{ // case for all points on the boundaries
				int idx = (row*nCols) + col; // index of current location
				newState[idx] = thisSim->bdryVal; 
			}
		}
	}

	// Now that the new state is all updated and the prior state is no longer needed,
	// copy the values in newState into priorState, and move onto the next time step.
	thisSim->currentTimeIdx = thisSim->currentTimeIdx + 1; // have completed a time step
	for(row=0; row<nRows; ++row){
		for(col=0; col<nCols; ++col){
			int idx = (row*nCols) + col;
			priorState[idx] = newState[idx];
		}
	}

	// since no problems were found earlier, return a 0
	return 0;

};

// Simulate nSteps time steps and record snapshots of the whole temperature field
// every stepsPerCheckPt time steps. runSim does do the initialization of the checkPtTime
// struct automatically at the beginning of the simulation.
// Note: running the simulation doesn't also initialize the sim or the material. Do them separately.
int runSim(sim *thisSim, int nSteps, int stepsPerCheckPt, checkPtTime *theseTimes){
	int flag = 0; // return flag, 0 if no problem, but nonzero if there's a problem

	// do the initialization of the checkpointing struct
	int nSnaps = calcNSnaps(nSteps,stepsPerCheckPt);
	int checkPtInitFlag = initCheckPtTime(theseTimes, thisSim->thisMaterial, thisSim, nSnaps);
	if(checkPtInitFlag){
		printf("WARNING: issue initializing checkpoint in runSim \n");
		flag =  checkPtInitFlag;
	}

	// run through the steps
	int step;
	recordSnap(theseTimes); // always record 0th  time step
	for(step=1; step<nSteps; ++step){
		int stepFlag = oneStep(thisSim);
		if(stepFlag){
			printf("WARNING: issue in simulation at %d time step \n",step);
			flag = stepFlag;
		}
		if(step % stepsPerCheckPt == 0) recordSnap(theseTimes); // this checks if a checkpoint needs to be recorded, and records it if needed
	}
	return flag;

};

// deallocate memory associated with currentState, priorState, initState and all boundary conditions
int cleanupSim(sim *thisSim){
	free(thisSim->initState);
	thisSim->initState = NULL;
	free(thisSim->priorState);
	thisSim->priorState = NULL;
	free(thisSim->currentState);
	thisSim->currentState = NULL;
	return 0;
};
