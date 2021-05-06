#ifndef __MATERIALSER_H__
#define __MATERIALSER_H__
typedef struct material_struct{
	// information inherent to the material itself
	unsigned int Nx; // number of columns in material grid
	float dx; // spacing (meters) between spatial grid points in x direction
	unsigned int Ny; // number of rows in material grid
	float dy; // spacing (meters) between spatial grid points in y direction
	float alpha; // homogeneous diffusivity of the medium
} material;

int initMaterial(material *aMaterial, unsigned int Nx, unsigned int Ny, float dx, float dy, float alpha);
#endif
