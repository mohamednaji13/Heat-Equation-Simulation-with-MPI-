#include <stdio.h>
#include <stdlib.h>
#include "checkPtPar.h"
#include "materialPar.h"
#include "simulationPar.h"
#include <mpi.h>


// initialize the space for times and stateSnapshots (note: assumes thisMaterial already initialized)
int initCheckPtTimeLoc(checkPtTimeLoc *thisCheckPtLoc, materialLoc *thisMaterialLoc, simLoc *thisSimLoc, int nSnaps){
    
	thisCheckPtLoc->nSnaps = nSnaps; // number of time snapshots to be recorded
	thisCheckPtLoc->times = (float *)malloc(nSnaps*sizeof(float));
	thisCheckPtLoc->currentSnapIdx = 0; // start out on the 0th snapshot
	thisCheckPtLoc->thisMaterialLoc = thisMaterialLoc; // set a pointer to this material so you can always grab number of points in space
	int nSpacePts = thisMaterialLoc->Nx * thisMaterialLoc->NyLocal; // number of points in space per local snapshot
	thisCheckPtLoc->stateSnapshotsLoc = (float *)malloc(nSnaps*nSpacePts*sizeof(float)); 
	thisCheckPtLoc->thisSimLoc = thisSimLoc; // set a pointer to this local part of simulation os you can always get access to the simulation's current state and time

	int flag = 0;
	if((thisCheckPtLoc->times == NULL) || (thisCheckPtLoc->stateSnapshotsLoc == NULL)){
		printf("WARNING: in initCheckPtTimeLoc, issue initializing local snapshot or time arrays \n");
		flag = 1;
	}
	return flag;
};

// Calculate the number of snapshots you'll make if you start at the
// 0th time step as the 0th snapshot, and take a new snapshot every 
// stepsPerCheckPt time steps in a simulation that will have nSteps
// including the 0th time step.
int calcNSnapsLoc(int nSteps, int stepsPerCheckPt){
	if(nSteps % stepsPerCheckPt == 0){
		return nSteps/stepsPerCheckPt;
	}
	else{
		return 1+(nSteps/stepsPerCheckPt);
	}
};

// record the current local snapshot
int recordSnapLoc(checkPtTimeLoc *thisCheckPtLoc){
	int flag = 0; // will be nonzero if there's an issue
	if((thisCheckPtLoc->times == NULL) || (thisCheckPtLoc->stateSnapshotsLoc == NULL)){
		printf("WARNING: recordSnap was called on a check point with uninitialized arrays \n");
		flag = 1;
	}

	// record the current time in seconds of this snapshot
	int currentId = thisCheckPtLoc->currentSnapIdx;
	thisCheckPtLoc->times[currentId] = (float)((thisCheckPtLoc->thisSimLoc)->currentTimeIdx) * (thisCheckPtLoc->thisSimLoc)->dt;

	// --------record the current snapshot of the temperature field---------
	// get a pointer to the beginning of the part of the local state array for this simulation where you'll start recording (nPadRows * # of columns after start)
	int nPadEntries = (thisCheckPtLoc->thisMaterialLoc)->Nx * (thisCheckPtLoc->thisMaterialLoc)->nPadRows; // number of entries to ignore at start of local padded state array 
	float *currentSnapshotLoc = (thisCheckPtLoc->thisSimLoc)->priorStateLoc + nPadEntries; // pointer to the local current state in the simulation
    // get a pointer to the beginning of the overall local state snapshots where to record this local snapshot
	int nSpacePts = (thisCheckPtLoc->thisMaterialLoc)->Nx * (thisCheckPtLoc->thisMaterialLoc)->NyLocal; // number of points in space per local snapshot
	int startID = nSpacePts * currentId; // current index within stateSnapshots to start
	float *start = thisCheckPtLoc->stateSnapshotsLoc + startID; // beginning of the current snapshot in thisCheckPt
	// actually copy entries of the current temperature field form the simulation to the checkPtTime's array
	int i;
	for(i=0; i<nSpacePts; ++i){
        start[i] = currentSnapshotLoc[i];
	}

	// next one you'll record will be the next snapshot index, so move along
	thisCheckPtLoc->currentSnapIdx = currentId + 1;
	
	return flag;
};

// Have rank 0 write all snapshots to a file at end of simulation (file named as filename). 
// Data will be in form:
// Nx
// Ny
// nSnaps
// 1stSnapTime, all, entries, of, first, snapshot, in, order, in, one, row
// 2ndSnapTime, all, entries, of, second, snapshot, in, order, in, one, row
// etc...
int writeToFileLoc(checkPtTimeLoc *thisCheckPtLoc, const char *filename){
    int flag = 0;
    int root = 0; // root rank to do the writing
    // check rank and number of processes
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    
    // create array with all receive counts and starting indices to be used in gatherv calls
    int *recvCounts = (int *)malloc(size*sizeof(int));
    int *displacements = (int *)malloc(size*sizeof(int));
    int r;
    int NyTotal = (thisCheckPtLoc->thisMaterialLoc)->NyTotal;
    int Nx = (thisCheckPtLoc->thisMaterialLoc)->Nx;
    int counter = 0;
    for(r=0; r < size; ++r){ 
        displacements[r] = counter; // index of start of where data will be recorded from the r^th process
        recvCounts[r] = Nx* (NyTotal/size); // number of entries to expect from the r^th process
        if(r < (NyTotal % size)) recvCounts[r] = Nx * ((NyTotal/size)+1); // extra row if not evenly divisible
        counter += recvCounts[r]; // add number of entries expected from this process onto the counter of all entries to fill in next displacement spot
    }

    
    if(rank == root){
        // get total number of space and time points over entire material over all processes (assume same snap times across all processes)
        int nLocalPts = Nx * (thisCheckPtLoc->thisMaterialLoc)->NyLocal;
        int nSpacePts = Nx * NyTotal;
        int nPtsTotal = nSpacePts * thisCheckPtLoc->nSnaps;
        float *totalSnapshots = (float *)malloc(nPtsTotal*sizeof(float));
        int snap;
        int nSnaps = thisCheckPtLoc->nSnaps;
        // for each snapshot gather (using MPI_Gatherv) the parts of the snapshot on the root rank
        for(snap=0; snap<nSnaps; ++snap){
            float *sendPtr = thisCheckPtLoc->stateSnapshotsLoc + (nLocalPts*snap);
            flag += MPI_Gatherv(sendPtr, nLocalPts, MPI_FLOAT, totalSnapshots+(snap*nSpacePts), recvCounts, displacements, MPI_FLOAT, root, MPI_COMM_WORLD);
        }
    
        // open up the file to write into
	    FILE *filePtr;
	    filePtr = fopen(filename, "w");
	    if(filePtr == NULL){
	       	printf("ERROR in opening file in writeToFile \n");
	    	flag += 1;
	    	return flag;
    	}
    	// write the first few lines saying the dimensions of the 3D array you're about to put  in the file
    	int Nx = (thisCheckPtLoc->thisMaterialLoc)->Nx;
    	fprintf(filePtr, "%d\n",Nx);
    	int NyTotal = (thisCheckPtLoc->thisMaterialLoc)->NyTotal;
    	fprintf(filePtr, "%d\n",NyTotal);
    	fprintf(filePtr, "%d\n",nSnaps);
    	
    	// write each snapshot
    	int k;
    	for(snap=0; snap<nSnaps; ++snap){
    		// write the time of the simulation
		    fprintf(filePtr,"%f ,",thisCheckPtLoc->times[snap]);
	    	// write the overall state in a flattened array at that time
	    	for(k=0; k<nSpacePts; ++k){
	    		fprintf(filePtr," %f ,",totalSnapshots[k+(snap*nSpacePts)]);
	    	}
	    	fprintf(filePtr,"\n");
	    }
	    fclose(filePtr);
	    
	    // cleanup the snapshot array
	    free(totalSnapshots);
	    totalSnapshots = NULL;
    }
    else{ // all other ranks just participate in the gather of local state snapshots into overall state snapshots at each snapshot
        float *totalSnapshots; // just a placeholder on these ranks
        int snap;
        int nSnaps = thisCheckPtLoc->nSnaps;
        int nLocalPts = (thisCheckPtLoc->thisMaterialLoc)->Nx * (thisCheckPtLoc->thisMaterialLoc)->NyLocal;
        // for each snapshot gather the local unpadded subarrays into the total snapshot array on the root process using MPI_Gatherv
        for(snap=0; snap<nSnaps; ++snap){
            float *sendPtr = thisCheckPtLoc->stateSnapshotsLoc + (nLocalPts*snap);
            flag += MPI_Gatherv(sendPtr, nLocalPts, MPI_FLOAT, totalSnapshots, recvCounts, displacements, MPI_FLOAT, root, MPI_COMM_WORLD);
        }
        
    }
    
    // cleanup recvCounts and displacements arrays used for gatherv's
    free(recvCounts);
    recvCounts = NULL;
    free(displacements);
    displacements = NULL;

	return flag; 
};

// cleanup space  allocated for times and stateSnapshots in checkPtTime struct
int cleanupCheckPtTimeLoc(checkPtTimeLoc *thisCheckPtLoc){
	free(thisCheckPtLoc->times);
	thisCheckPtLoc->times = NULL;
	free(thisCheckPtLoc->stateSnapshotsLoc);
	thisCheckPtLoc->stateSnapshotsLoc = NULL;
	return 0;
};
