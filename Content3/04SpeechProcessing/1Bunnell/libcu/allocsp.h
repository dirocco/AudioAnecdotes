/* Allocsp.h */
#ifndef __ALLOCSP_HEADER__
#define __ALLOCSP_HEADER__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __WIN16__
typedef long ALLOCSZ;
#else
typedef int ALLOCSZ;
#endif

DLL_PREFIX char  **alloc2d (ALLOCSZ dim1, ALLOCSZ dim2, ALLOCSZ size);
int DLL_PREFIX free2d (void **mat);
DLL_PREFIX char ***alloc3d (ALLOCSZ dim1, ALLOCSZ dim2, ALLOCSZ dim3, ALLOCSZ size);
int DLL_PREFIX free3d (void ***mat);
#ifdef __cplusplus
}
#endif
#endif
