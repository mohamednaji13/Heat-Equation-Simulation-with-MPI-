#ifndef __SIMULATIONSER_H__
#define __SIMULATIONSER_H__

// forward declarations of structs a sim will have pointers to
typedef struct material_struct material;
typedef struct checkPtTime_struct checkPtTime;

typedef struct sim_struct{	
	// We'll always be looking at du/dt = alpha * (d^2u/dx^2 + d^2u/dy^2)
	// so here are some data specific to the material for that simulation. 
	
	float dtMax; // the maximum allowed finite difference time step for stability of the simulation
	float dt; // the actual timestep set by the user
	
	material *thisMaterial; // a pointer to a material that already has its data and diffusivity array filled in
	
	// These get updated at each time step of the simulation
	unsigned int currentTimeIdx; // integer saying which time step the simulation is on for currentState (start at 0, then 1, then 2, ... and corresponding times in seconds are 0, dt, 2*dt, etc...)
	float *currentState; // a pointer to the current temperature matrix (thisMaterial.Nx x thisMaterial.Ny points) 	
	float *priorState; // a pointer to the prior temperature matrix (thisMaterial.Nx x thisMaterial.Ny points)

	// initial conditions and boundary value
	float *initState; // initial temperature state
	float bdryVal; // a single float that will be the constant temperature value around all boundary points (all edges of the material)

} sim;

// Calculate the maximum stable time step allowed by the CFL condition
float calcMaxTimeStep(sim *thisSim);

// Initialize the initial state (temperature matrix at T = 0, copied from valsForInitState) and current state of the system (matrix of temperature values).
// Note: initializing the simulation does not also initialize the material. Do that separately.
int initSim(sim *thisSim, float timeStep, float *valsForInitState, float bdryVal, material *thisMaterial);

// Move the simulation forward by one time step
int oneStep(sim *thisSim);

// Simulate nSteps time steps and record snapshots of the whole temperature field
// every stepsPerCheckPt time steps. runSim does do the initialization of the checkPtTime
// struct automatically at the beginning of the simulation.
// Note: running the simulation doesn't also initialize the sim or the material. Do them separately.
int runSim(sim *thisSim, int nSteps, int stepsPerCheckPt, checkPtTime *theseTimes);

// deallocate memory associated with currentState, initState and all boundary conditions
int cleanupSim(sim *thisSim);

#endif
