#ifndef __MATERIALPAR_H__
#define __MATERIALPAR_H__
typedef struct materialLoc_struct{
	// information inherent to the material itself
	unsigned int Nx; // number of columns in material grid
	float dx; // spacing (meters) between spatial grid points in x direction
	unsigned int NyTotal; // number of rows in overall global material grid
	unsigned int NyLocal; // number of rows in this local unpadded subset of the material grid
	unsigned int startYId; // index of the lowest row index in the unpadded subset of the material grrid as it would be positioned within the global material grid
	unsigned int nPadRows; // number of rows of padding on each side (so 2*nPadRows + Nylocal = NyPadded)
	unsigned int NyPadded; // number of rows in this local padded subset of the material grid
	float dy; // spacing (meters) between spatial grid points in y direction
	float alpha; // homogeneous diffusivity of the medium
} materialLoc;

int initMaterialLoc(materialLoc *aMaterial, unsigned int Nx, unsigned int NyTotal, unsigned int nPadRows, float dx, float dy, float alpha);
#endif
