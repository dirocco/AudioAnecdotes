#ifndef __LIBCU_HEADER__
#define __LIBCU_HEADER__


#ifdef __cplusplus
extern "C" {
#endif

#ifdef DLL_LIB
#define DLL_PREFIX __declspec(dllexport)
#else
#define DLL_PREFIX 
#endif
/*
 * These header files need the DLL_PREFIX definition
 */
#include "rflio.h"
#include "unpack.h"
#include "ann.h"
#include "allocsp.h"

int DLL_PREFIX dcs (char *dc, int *ipt, char *ins, char *ous, char *ch);
void DLL_PREFIX defext (char *str, char *ext);
void DLL_PREFIX setext (char *str, char *ext);
DLL_PREFIX char *stripext(char *fn);
DLL_PREFIX char *jscat ( /* char *rtn, char *in1, ..., char *(null) */ );
#ifdef LOG2
double DLL_PREFIX log2(double value);
#endif
float DLL_PREFIX pearsn(float x[], float y[], int n);
void DLL_PREFIX prompt (char *p, char *r);
int DLL_PREFIX StdDev( int n, float *Data, float *Scaled, double Mult);
void DLL_PREFIX strpad (char *str, int length);
DLL_PREFIX char *uatola(char *s);
DLL_PREFIX char *latoua(char *s);
#ifdef __cplusplus
}
#endif

#endif
