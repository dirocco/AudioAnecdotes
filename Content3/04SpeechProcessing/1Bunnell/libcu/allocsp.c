#include <stdio.h>
#include <malloc.h>
#include "libcu.h"

/*
 * Matrix allocation routines.  To allocate a 10 by 10 matrix of floats use:
 * 
 * char **alloc2d(); float **x;
 * 
 * x = (float **) alloc2d(10, 10, sizeof(float));
 * 
 * To free this matrix use:
 * 
 * free2d(x);
 */

/*
 *--Allocate & free 2D arrays
 */

DLL_PREFIX char  **alloc2d (ALLOCSZ dim1, ALLOCSZ dim2, ALLOCSZ size)
{
	ALLOCSZ    i;
	ALLOCSZ    nelem;
	char       *p, **pp;

	nelem = dim1 * dim2;

	if (!(p = calloc (nelem, size))) 
		return (char **) NULL;
		
	if (!(pp = (char **) calloc (dim1, (ALLOCSZ) sizeof (char *)))) {
		free ((char *) p);
		return (char **) NULL;
	}
	for (i = 0; i < dim1; i++)
		pp[i] = p + i * dim2 * size;

	return (pp);
}

int DLL_PREFIX free2d (void **mat)
{
	free ((void *) *mat);
	free ((void *) mat);
	return (0);
}

/*
 *--Allocate & freee 3D array
 */

DLL_PREFIX char ***alloc3d (ALLOCSZ dim1, ALLOCSZ dim2, ALLOCSZ dim3, ALLOCSZ size)
{
	ALLOCSZ  i;
	char **pp, ***ppp;

	if (!(pp = (char **) alloc2d (dim1 * dim2, dim3, size)))
		return (char ***) NULL;

	if (!(ppp = (char ***) calloc ((ALLOCSZ) dim1, (ALLOCSZ) sizeof (char **)))) {
		free2d ((void **) pp);
		return (char ***) NULL;
	}

	for (i = 0; i < dim1; i++)
		ppp[i] = pp + i * dim2;
	return (ppp);
}

int DLL_PREFIX free3d (void ***mat)
{
	free ((void *) **mat);
	free ((void *) *mat);
	free ((void *) mat);
	return (0);
}
