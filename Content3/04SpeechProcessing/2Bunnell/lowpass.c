#include <stdlib.h>
#include <math.h>
#include "dsp.h"

float *lowpass(int nin, short *rawwav, double cut)
{
  int nfft, icut, j, k, nfo2;
  double rinc, wt, rad;
  float *fltwav;

/*
 * Get transform length. The transform buffer should be at least
 * twice the length of the input sequence so that ripple from the
 * filtering operation falls outside the area containing the
 * input sequence.
 */

  nfft = 2;
  while (nfft < nin)  /* Get the minimum necessary length */
    nfft += nfft;
  nfft += nfft;		/* Double that for the actual transform */

  fltwav = (float *) malloc((nfft+2) * sizeof(float));
  nfo2 = nfft / 2;

/*
 * radian increment for filter response
 */

  icut = (int) (cut*nfo2 + 0.5);
  if (icut >= nfft)
    return (float *) NULL;

  rinc = (float) (3.14159 / (icut*2));

/*
 * copy data to floating point array and pad with zeroes
 */

  for (j=0; j<nin; ++j)
    fltwav[j] = (float) rawwav[j];

  if (nfft > nin) {
    for (j=nin; j<nfft+2; ++j)
      fltwav[j] = (float)0.0;
  }

/*
 * Transform
 */
  ffa(fltwav, nfft);

/*
 * Multiply by filter response
 */
  for (j = 0, rad=(float)0.0; j < nfo2; ++j, rad+=rinc) {
    k = (j*2) + 1;
    if (rad < 3.14159) {
      wt = (float) (cos(rad) * 0.5 + 0.5);
    } else {
      wt = (float) 0.0;
    }
    fltwav[k] = (float) (wt * fltwav[k]);
    fltwav[k+1] = (float) (wt * fltwav[k+1]);
    rad += rinc;
  }

/*
 * Invert the transform
 */
  ffsy(fltwav, nfft);

  return fltwav;
}
