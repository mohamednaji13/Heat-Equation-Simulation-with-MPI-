#ifndef __CHECKPTSER_H__
#define __CHECKPTSER_H__

// forward declarations of structs a checkPtTime will have pointers to
typedef struct sim_struct sim;
typedef struct material_struct material;

typedef struct checkPtTime_struct{
	material *thisMaterial; // pointer to an already initialized material grid
	sim *thisSim; // a pointer to an already initialized simulation
	int nSnaps; // number of snapshots to record
	int currentSnapIdx; // index of the current snapshot (within times and stateSnapshots)
	float *times; // record times (in seconds) of each snapshot (nSnaps entries)	
	float *stateSnapshots; // pointer to the snapshots (nSnaps x thisMaterial.Nx x thisMaterial.Ny
} checkPtTime;

// Calculate the number of snapshots you'll make if you start at the
// 0th time step as the 0th snapshot, and take a new snapshot every 
// stepsPerCheckPt time steps in a simulation that will have nSteps
// including the 0th time step.
int calcNSnaps(int nSteps, int stepsPerCheckPt);

// initialize the space for times and stateSnapshots (note: assumes thisMaterial already initialized)
int initCheckPtTime(checkPtTime *thisCheckPt, material *thisMaterial, sim *thisSim, int nSnaps);

// record the current snapshot
int recordSnap(checkPtTime *thisCheckPt);

// write all snapshots to a file at end of simulation (file named as filename). 
// Data will be in form:
// Nx
// Ny
// nSnaps
// 1stSnapTime, all, entries, of, first, snapshot, in, order, in, one, row
// 2ndSnapTime, all, entries, of, second, snapshot, in, order, in, one, row
// etc...
int writeToFile(checkPtTime *thisCheckPt, const char *filename);

// cleanup space  allocated for times and stateSnapshots in checkPtTime struct
int cleanupCheckPtTime(checkPtTime *thisCheckPt);
#endif
