#ifndef __SIMULATIONPAR_H__
#define __SIMULATIONPAR_H__

// forward declarations of structs a sim will have pointers to
typedef struct materialLoc_struct materialLoc;
typedef struct checkPtTimeLoc_struct checkPtTimeLoc;

typedef struct simLoc_struct{	
	// We'll always be looking at du/dt = alpha * (d^2u/dx^2 + d^2u/dy^2)
	// so here are some data specific to the material for that simulation. 
	
	float dtMax; // the maximum allowed finite difference time step for stability of the simulation
	float dt; // the actual timestep set by the user
	
	materialLoc *thisMaterialLoc; // a pointer to the local subset of a material that already has its parameters filled in
	
	// These get updated at each time step of the simulation
	unsigned int currentTimeIdx; // integer saying which time step the simulation is on for currentState (start at 0, then 1, then 2, ... and corresponding times in seconds are 0, dt, 2*dt, etc...)
	float *currentStateLoc; // a pointer to the current local temperature matrix (thisMaterial.Nx x thisMaterial.NyPadded points) 	
	float *priorStateLoc; // a pointer to the prior local temperature matrix (thisMaterial.Nx x thisMaterial.NyPadded points)

	// initial conditions and boundary value
	float *initStateLoc; // initial temperature state in this local region (thisMaterial.Nx x thisMaterial.NyLocal points)
	float bdryVal; // a single float that will be the constant temperature value around all boundary points (all edges of the global material, and at least the 0th and last columns of this local submaterial)

} simLoc;

// Calculate the maximum stable time step allowed by the CFL condition
float calcMaxTimeStepLoc(simLoc *thisSimLoc);

// Initialize the loccal initial state (temperature matrix at T = 0, copied from valsForInitStateGlobal) 
// and current state of the system (matrix of temperature values).
// valsForInitStateGlobal will be thisMaterial.Nx x thisMaterial.NyTotal, and only the values relevant 
// to this process's local subset of the material should be copied in. 
// Note: initializing the simulation does not also initialize the material. Do that separately.
int initSimLoc(simLoc *thisSimLoc, float timeStep, float *valsForInitStateGlobal, float bdryVal, materialLoc *thisMaterialLoc);

// Share ghost region information (must be done before each step of the simulation). Send first
// row of unpadded part of local state to previous process (except rank 0), and send last row
// of unpadded part of local state to next process (except last rank). 
int exchangeGhostRegions(simLoc *thisSimLoc);

// Update ghost regions and move the simulation forward by one time step in this local region 
int oneStepLoc(simLoc *thisSimLoc);

// Simulate nSteps time steps and record snapshots of the whole temperature field
// every stepsPerCheckPt time steps. runSimLoc does do the initialization of the checkPtTimeLoc
// struct automatically at the beginning of the simulation. 
// Note: running the simulation doesn't also initialize the sim or the material. Do them separately.
int runSimLoc(simLoc *thisSimLoc, int nSteps, int stepsPerCheckPt, checkPtTimeLoc *theseTimesLoc);

// deallocate memory associated with currentStateLoc, initStateLoc, priorStateLoc
int cleanupSimLoc(simLoc *thisSimLoc);

#endif
