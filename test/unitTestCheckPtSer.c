#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../code/materialSer.h"
#include "../code/checkPtSer.h"
#include "../code/simulationSer.h"

// keeps track of tests passed, failed, and current test index
void incrementTestCtr(int flag, int *nTestsPassed, int *nTestsFailed, int *testID){
	if(flag == 0){
		*nTestsPassed = *nTestsPassed + 1;
	}
	else{
		*nTestsFailed = *nTestsFailed + 1;
	}
	*testID = *testID + 1;
	return;
};

// we'll reuse the same setup for multiple tests
int setup(material *aMaterial, checkPtTime *aCheckPt, sim *aSim){
	int Nx = 8;
	int Ny = 10;
	float dx = 0.7;
	float dy = 0.6;
	float alpha = 0.5;
	int flag = initMaterial(aMaterial,Nx,Ny,dx,dy,alpha);

	// setup simulation
	float dt = 0.15;
	float boundary = 1;
	float *initTemp = malloc(Nx*Ny*sizeof(float));
	int j,k;
	for(j=0; j<Ny; ++j){
		for(k=0; k<Nx; ++k){
			if((0<k) && (k<Nx-1) && (0<j) && (j<Ny-1)){
				initTemp[k + j*Nx] = 2; // set interior pts
			}
			else{
				initTemp[k + j*Nx] = boundary; // set bdry pts
			}
		}
	}
	flag += initSim(aSim, dt, initTemp, boundary, aMaterial);
	free(initTemp);
	initTemp = NULL;

	// setup checkpoints
	int nSteps = 10;
	int stepsPerCheckPt = 2;
	int nSnaps = calcNSnaps(nSteps, stepsPerCheckPt);
	flag += initCheckPtTime(aCheckPt, aMaterial, aSim, nSnaps);

	return flag;
};

int testNSnaps(int testID){
	int nSteps = 11;
	int stepsPerCheckPt = 5;
	int nSnaps = calcNSnaps(nSteps, stepsPerCheckPt);
	// shoudl expect checkpoint snapshots at steps 0, 5, 10 (so 3 total)
	if(nSnaps != 3){
		printf("Failed test %d \n",nSnaps);
		return 1;
	}
	else{
		printf("Passed test %d \n",testID);
		return 0;
	}
};

// ---------------------DEFINE YOUR TESTS HERE------------------

// ------------------------------------------------------------

// The actual main function that runs all tests
int main(){
	// this is a series  of unit tests of creating a material
	int nTestsPassed = 0;
	int nTestsFailed = 0;
	int testID = 0;
	int flag; // each test will return a 0 if passed and a 1 if failed

	
	flag = testNSnaps(testID); 
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);
	
	//  -----------ADD YOUR TEST CALLS HERE--------------------

	//  ------------------------------------------------------


	printf("----CHECKPOINT UNIT TESTS----\n");
	printf("----------SUMMARY----------\n");
	printf("Tests passed: %d \n",nTestsPassed);
	printf("Tests failed: %d \n",nTestsFailed);

	return 0;
}
