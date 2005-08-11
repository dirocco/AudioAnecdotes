#include <math.h>
#include "Precisn.h"
#define RTPIO2 1.2533141

void sphbes(int n, FLOAT x, FLOAT *sj, FLOAT *sy, FLOAT *sjp, FLOAT *syp)
{
	void bessjy(FLOAT x, FLOAT xnu, FLOAT *rj, FLOAT *ry, FLOAT *rjp,
		FLOAT *ryp);
	void nrerror(char error_text[]);
	FLOAT factor,order,rj,rjp,ry,ryp;

	if (n < 0 || x <= 0.0) nrerror("bad arguments in sphbes");
	order=n+0.5;
	bessjy(x,order,&rj,&ry,&rjp,&ryp);
	factor=RTPIO2/sqrt(x);
	*sj=factor*rj;
	*sy=factor*ry;
	*sjp=factor*rjp-(*sj)/(2.0*x);
	*syp=factor*ryp-(*sy)/(2.0*x);
}
#undef RTPIO2
