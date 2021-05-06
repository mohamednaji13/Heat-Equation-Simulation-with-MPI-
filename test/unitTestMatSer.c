#include <stdio.h>
#include <math.h>
#include "../code/materialSer.h"

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
int setup(material *aMaterial){
	int Nx = 8;
	int Ny = 10;
	float dx = 0.7;
	float dy = 0.6;
	float alpha = 0.5;
	int flag = initMaterial(aMaterial,Nx,Ny,dx,dy,alpha);
	return flag;
};

// test whether Nx is set properly
int testNx(int testID){
	material thisMaterial;
	int flag = setup(&thisMaterial);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	if(thisMaterial.Nx != 8){ 
		printf("ERROR in test %d , wrong Nx value \n",testID);
		return 2; 
	}
	else{	
		printf("Test %d passed.\n",testID);
		return 0;
	}
};

// test whether Ny is set properly
int testNy(int testID){
	material thisMaterial;
	int flag = setup(&thisMaterial);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	if(thisMaterial.Ny != 10){ 
		printf("ERROR in test %d , wrong Ny value \n",testID);
		return 2; 
	}
	else{	
		printf("Test %d passed.\n",testID);
		return 0;
	}
};

// test whether dx is set properly
int testdx(int testID){
	material thisMaterial;
	int flag = setup(&thisMaterial);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	double diff = fabs(thisMaterial.dx - 0.7);
	if(diff > 1e-6){ 
		printf("ERROR in test %d , wrong dx value \n",testID);
		return 2; 
	}
	else{	
		printf("Test %d passed.\n",testID);
		return 0;
	}
};

// test whether dy is set properly
int testdy(int testID){
	material thisMaterial;
	int flag = setup(&thisMaterial);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	double diff = fabs(thisMaterial.dy - 0.6);
	if(diff > 1e-6){ 
		printf("ERROR in test %d , wrong dy value \n",testID);
		return 2; 
	}
	else{	
		printf("Test %d passed.\n",testID);
		return 0;
	}
};

// test whether alpha is set properly
int testAlpha(int testID){
	material thisMaterial;
	int flag = setup(&thisMaterial);
	if(flag != 0){ 
		printf("ERROR in test %d ,  initialization issue \n",testID);
		return 1;
	}
	double diff = fabs(thisMaterial.alpha - 0.5);
	if(diff > 1e-6){ 
		printf("ERROR in test %d , wrong alpha value \n",testID);
		return 2; 
	}
	else{	
		printf("Test %d passed.\n",testID);
		return 0;
	}
};



// The actual main function that runs all tests
int main(){
	// this is a series  of unit tests of creating a material
	int nTestsPassed = 0;
	int nTestsFailed = 0;
	int testID = 0;
	int flag; // each test will return a 0 if passed and a 1 if failed

	
	flag = testNx(testID); 
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);
	
	flag = testNy(testID);
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);
	
	flag = testdx(testID); 
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);
	
	flag = testdy(testID);
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);

	flag = testAlpha(testID);
	incrementTestCtr(flag, &nTestsPassed, &nTestsFailed, &testID);

	printf("----MATERIAL UNIT TESTS----\n");
	printf("----------SUMMARY----------\n");
	printf("Tests passed: %d \n",nTestsPassed);
	printf("Tests failed: %d \n",nTestsFailed);

	return 0;
}
