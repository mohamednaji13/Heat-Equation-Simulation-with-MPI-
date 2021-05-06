#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../code/materialSer.h"
#include "../code/simulationSer.h"
#include "../code/checkPtSer.h"

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
int setup(material *aMaterial, sim *aSim){
	// setup the material
	int Nx = 8;
	int Ny = 10;
	float dx = 0.7;
	float dy = 0.6;
	float alpha = 0.5;
	int flag = initMaterial(aMaterial,Nx,Ny,dx,dy,alpha);

	// setup the simulation
	float dt = 0.15;
	float boundary = 1;
	float *initTemp = malloc(Nx*Ny*sizeof(float));
	int j,k;
	for(j=0; j<Ny; ++j){
		for(k=0; k<Nx; ++k){
			if((0<k) &&(k<Nx-1) && (0<j) && (j<Ny-1)){
				initTemp[k+ j*Nx] = 2; // make all interior points 2
			}
			else{
				initTemp[k + j*Nx] = boundary; // set all boundary points to 1
			}
		}
	}
	
	flag += initSim(aSim, dt, initTemp, boundary, aMaterial);

	free(initTemp);
	initTemp = NULL;
	return flag;
};

// test whether DtMax is set properly
int testDtMax(int testID){
	material thisMaterial;
	sim thisSim;
	int flag = setup(&thisMaterial, &thisSim);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	if(fabs(thisSim.dtMax-0.18) > (1e-6)*0.18){ 
		printf("ERROR in test %d , wrong dtMax value \n",testID);
		return 2; 
	}
	else{	
		printf("Test %d passed.\n",testID);
		return 0;
	}
};

// test whether dt is set properly
int testDt(int testID){
	material thisMaterial;
	sim thisSim;
	int flag = setup(&thisMaterial, &thisSim);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	if(fabs(thisSim.dt - 0.15)  > (1e-6)*0.15){ 
		printf("ERROR in test %d , wrong dt value \n",testID);
		return 2; 
	}
	else{	
		printf("Test %d passed.\n",testID);
		return 0;
	}
};

// test whether dx is set properly
int testBdry(int testID){
	material thisMaterial;
	sim thisSim;
	int flag = setup(&thisMaterial, &thisSim);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	double diff = fabs(thisSim.bdryVal - 1.0);
	if(diff > 1e-6){ 
		printf("ERROR in test %d , wrong boundary value \n",testID);
		return 2; 
	}
	else{	
		printf("Test %d passed.\n",testID);
		return 0;
	}
};

// test whether left bdry after one step is set properly
int testBdryStep(int testID){
	material thisMaterial;
	sim thisSim;
	int flag = setup(&thisMaterial, &thisSim);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	checkPtTime thisCheck;
	flag = runSim(&thisSim, 2, 1, &thisCheck);
	// check the value onn row 2 left side
	float leftVal = thisSim.priorState[8*2];
	double diff = fabs(leftVal - 1.0);
	if(diff > 1e-6){
		printf("ERROR in test %d , problem with left boundary value \n",testID);
		return 2; 
	}
	// check the value  on row 2 right side
	float rightVal = thisSim.currentState[8*3-1];
	diff = fabs(rightVal - 1.0);
	if(diff > 1e-6){
		printf("ERROR in test %d , problem with right boundary value \n",testID);
		return 3;
	}
	// check the value on 0th row 3rd entry
	float topVal = thisSim.currentState[3];
	diff = fabs(topVal - 1.0);
	if(diff > 1e-6){
		printf("ERROR in test %d , problem with top boundary value \n",testID);
		return 4;
	}
	// check the value on bottom row 3rd entry
	float bottomVal = thisSim.currentState[8*9+3];
	diff = fabs(bottomVal - 1.0);
	if(diff > 1e-6){
		printf("ERROR in test %d , problem with bottom boundary value \n",testID);
		return 5;
	}
	
	// only get to this point if all parts passed
	printf("Test %d passed.\n",testID);
	return 0;
};


// test whether (1,1) point after one step is set properly
int testIntStep(int testID){
	material thisMaterial;
	sim thisSim;
	int flag = setup(&thisMaterial, &thisSim);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	checkPtTime thisCheck;
	flag = runSim(&thisSim, 2, 1, &thisCheck);
	// check the value on row 1 column 1
	float thisTemp = thisSim.currentState[8*1 + 1];
	float expectedTemp = 1.638605; // calculated by hand for this first step
	double diff = fabs(thisTemp - expectedTemp);
	if(diff > 1e-4){
		printf("ERROR in test %d , problem with update of interior point \n",testID);
		return 2; 
	}
	
	// only get to this point if all parts passed
	printf("Test %d passed.\n",testID);
	return 0;
};




// The actual main function that runs all tests
int main(){
	// this is a series  of unit tests of creating a material
	int nTestsPassed = 0;
	int nTestsFailed = 0;
	int testID = 0;
	int flag; // each test will return a 0 if passed and a 1 if failed

	
	flag = testDtMax(testID); 
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);
	
	flag = testDt(testID);
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);
	
	flag = testBdry(testID); 
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);
	
	flag = testBdryStep(testID);
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);

	flag = testIntStep(testID);
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);

	printf("----SIMULATION UNIT TESTS----\n");
	printf("----------SUMMARY----------\n");
	printf("Tests passed: %d \n",nTestsPassed);
	printf("Tests failed: %d \n",nTestsFailed);

	return 0;
}
