
/* STDDEV -  scales 'n' samples of data pointed to by 'Data' to their
   standard deviation multiplied by 'Mult'. The scaled data are stored
   in 'Scaled'.

AUTHOR
   Mariusz Fecko
*/

#include <stdio.h>
#include <math.h>
#include "libcu.h"

int DLL_PREFIX StdDev( int n, float *Data, float *Scaled, double Mult)
{

register int i;
register double sum, mean;
double StandDev, InStandDev;

/* compute the mean value */

for( i=0, sum=0; i < n; ++i)
   sum += *( Data + i);
mean = sum / n;

/* compute the standard deviation */

for( i=0, sum=0; i < n; ++i)
  sum += *( Data +i) * *( Data+i);

StandDev = sqrt( (sum - mean*mean*n) / (n-1));
InStandDev = (StandDev > 1.0e-30)? 1.0 / (Mult*StandDev) : 0.0f;

/* now scaling */

for( i=0; i < n; ++i)
  *( Scaled + i) = (float) (InStandDev * ( (double) *(Data+i) - mean));

return 1;
}

#ifdef _TEST_FUNCTION

float Data[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 };
float S[20];

main()
{
 int i;

 StdDev( 20, Data, S, 1);

 for( i=0; i < 20; ++i)
   printf("\n %f   %f", Data[i], S[i]);

 printf ("\n");

 StdDev( 20, Data, S, 2);

 for( i=0; i < 20; ++i)
   printf("\n %f   %f", Data[i], S[i]);
 printf ("\n");

}

#endif
