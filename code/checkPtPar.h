#ifndef __CHECKPTPAR_H__
#define __CHECKPTPAR_H__

// forward declarations of structs a checkPtTime will have pointers to
typedef struct simLoc_struct simLoc;
typedef struct materialLoc_struct materialLoc;

typedef struct checkPtTimeLoc_struct{
	materialLoc *thisMaterialLoc; // pointer to an already initialized local subset of the material grid
	simLoc *thisSimLoc; // a pointer to an already initialized local subset of the simulation
	int nSnaps; // number of snapshots to record
	int currentSnapIdx; // index of the current snapshot (within times and stateSnapshots)
	float *times; // record times (in seconds) of each snapshot (nSnaps entries)	
	float *stateSnapshotsLoc; // pointer to the local snapshots (nSnaps x thisMaterial.NyLocal x thisMaterial.Nx)
} checkPtTimeLoc;

// Calculate the number of snapshots you'll make if you start at the
// 0th time step as the 0th snapshot, and take a new snapshot every 
// stepsPerCheckPt time steps in a simulation that will have nSteps
// including the 0th time step.
int calcNSnapsLoc(int nSteps, int stepsPerCheckPt);

// initialize the space for times and stateSnapshotsLoc (note: assumes thisMaterialLoc already initialized)
int initCheckPtTimeLoc(checkPtTimeLoc *thisCheckPtLoc, materialLoc *thisMaterialLoc, simLoc *thisSimLoc, int nSnaps);

// record the current snapshot for this local subarray
int recordSnapLoc(checkPtTimeLoc *thisCheckPtLoc);

// Have rank 0 write all snapshots to a file at end of simulation after gathering snapshots (file named as filename). 
// Data will be in form:
// Nx
// Ny
// nSnaps
// 1stSnapTime, all, entries, of, first, snapshot, in, order, in, one, row
// 2ndSnapTime, all, entries, of, second, snapshot, in, order, in, one, row
// etc...
int writeToFileLoc(checkPtTimeLoc *thisCheckPtLoc, const char *filename);

// cleanup space  allocated for times and stateSnapshotsLoc in checkPtTimeLoc struct
int cleanupCheckPtTimeLoc(checkPtTimeLoc *thisCheckPtLoc);
#endif
