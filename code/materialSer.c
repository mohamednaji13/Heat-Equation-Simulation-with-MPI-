#include "materialSer.h"


// initialize the material (Nx x Ny) to have basic data, allocate space for an array of alpha value
int initMaterial(material *aMaterial, unsigned int Nx, unsigned int Ny, float dx, float dy, float alpha){
	// record spatial discretization parameters
	aMaterial->Nx = Nx;
	aMaterial->dx = dx;
	aMaterial->Ny = Ny;
	aMaterial->dy = dy;
	aMaterial->alpha = alpha; 
	return 0;
};
