#include <math.h>
#include "libcu.h"

float DLL_PREFIX pearsn(float x[], float y[], int n)
{
	int j;
	float yt,xt;
	float syy=0.0,sxy=0.0,sxx=0.0,ay=0.0,ax=0.0;

	for (j=0;j<n;j++) {
		ax += x[j];
		ay += y[j];
	}
	ax /= n;
	ay /= n;
	for (j=0;j<n;j++) {
		xt=x[j]-ax;
		yt=y[j]-ay;
		sxx += xt*xt;
		syy += yt*yt;
		sxy += xt*yt;
	}
	return sxy/(float)sqrt(sxx*syy);
}
