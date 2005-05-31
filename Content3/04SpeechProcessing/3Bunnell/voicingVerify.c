/*
*  voicingVerify.c - routines to estimate voicing, and compare with PPS
*
*	voicingEstimate takes a 40ms frame of speech and estimates whether it is voiced.
*		It returns a value in the range of 0 to 1 that gives likelihood of voicing.
*	
*	voicingVerify.c - Apply voicing detection to a sampled speech file 
*				to verify proper EGG operation
*	 Returns: max number of consecutive frames judged VOICED but with PPS "absent"
*
*		Silvio Eberhardt, 7/99	
*/

#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#ifdef MSC
#include <io.h>
#endif
#include <fcntl.h>
#include "wiodef.h"
#include "dsp.h"
#include "spenvel.h"
#include "pps.h"

#define WINDOW_SHORT 1	/* window data if format is short */
#define ABSVAL(i) ((i>0)?i:-i)
#define WINDOW_LENGTH 40.0 /* in ms */
#define FRAME_FREQ 100	/* in Hz */
#define THRESHOLD 0.65	/* actual decision threshold */
#define M_PI		3.14159265358979323846

/* FOR ADJUSTING RESULT TO MAKE DECISION POINT 0 AND +/-1 the standard deviations */
#define OFFSET 98.0f	
#define SCALE 2000.0f

/* COEFFICIENTS FOR SUM_OF_PRODUCT ESTIMATE GENERATION */
#define ZCDCOEFF	-4.094242f
#define F0COEFF		.8843684f
#define F1COEFF		1.93455f
#define F2COEFF		-.967643f
#define F4COEFF		.2327262f
#define F6COEFF		-.4362983f
#define F7COEFF		.4482865f
#define COMBCOEFF	-17.40898f

/*
------------------------------------------------------------------------------
  voiced |      Coef.   Std. Err.       z     P>|z|       [95% Conf. Interval]
---------+--------------------------------------------------------------------
     zcd |  -4.094242   .1946847    -21.030   0.000      -4.475817   -3.712667
      f1 |   .8843684   .0915885      9.656   0.000       .7048582    1.063879
      f2 |    1.93455   .1225111     15.791   0.000       1.694433    2.174668
      f3 |   -.967643   .1122161     -8.623   0.000      -1.187582   -.7477035
      f5 |   .2327262   .0857357      2.714   0.007       .0646874    .4007651
      f7 |  -.4362983   .1004055     -4.345   0.000      -.6330895   -.2395072
      f8 |   .4482865   .0874482      5.126   0.000       .2768912    .6196817
  cmbrat |   12.68526   .2075966     61.105   0.000       12.27838    13.09214
   _cons |  -17.40898   .2949134    -59.031   0.000        -17.987   -16.83096
------------------------------------------------------------------------------
*/

extern int combNoLog;

struct filters {
	float lo;
	float hi;
} filtFreqs[] = {
 	   0.0,  600.0,
	 600.0, 1200.0,
	1200.0, 1800.0,
	1800.0, 2400.0,
	2400.0, 3000.0,
	3000.0,	3600.0,
	3600.0, 4200.0,
	4200.0, 4800.0
};

#define NFILTERS 8  /* must be consistent with above (filtFreq) */


/* 
 *  Estimate whether the given 40ms section is speech is voiced or voiceless
 *	returns float in range 0 to 1 (approximating likelihood of being voiced).
 *
 *	Error return is -1.0 (couldn't malloc)
 *
 *	Coefficients were generated from 5 talkers (3 female w ages 32,32,14
 *	and 2 male ages 37, 48)
 *
 *	Note that the coefficients were set for 40 ms window, so coeff will
 *	have to be recalculated (with vda and statistical analysis) for different
 *	window lengths.
 *
 *		Silvio Eberhardt, 7/99
 *
 *	Added f0 return value so that both f0 & likelihood of voicing can be recovered
 *		Bunnell	7/00
 */

float voicingEstimate(float sampRate, void *data, double *f0, int isFloat)
{
	static int nfft=0, oldPts=0;	/* don't reallocate mem for fft unless changed */
	static float *c = (float *) NULL;
	int i,j,k,m,n,nconsecutive = 0, windowSize;
	float loc1=(float)0.0, loc2=(float)0.0, elapsed=(float)0.0;
	double rad,rinc;
	float *fl;
	double combEnergyRatio;
	int zc;
	float psum; // pitch-related metrics
	float filt, zcd, d1, nd1, f[NFILTERS];
	short *sh;

	windowSize = (int) (WINDOW_LENGTH / 1000.0 * (float)sampRate);
	for(nfft=128; windowSize > nfft; nfft+=nfft);

	if (!(c = (float *)malloc((nfft+4) * sizeof(float))))
		return -1.0;

	if(isFloat)
		fl = (float *) data;
	else
		sh = (short *) data;

	rinc = (double) (2.0*M_PI/(double)windowSize);

	for(i=0, rad=rinc; i<windowSize; i++, rad+=rinc)
		if(isFloat)		/* copy data, be it float or short */
			c[i] = fl[i];
		else			/* if short, window */
#ifdef WINDOW_SHORT
			c[i] = (float) ((double)sh[i]*(0.54 - 0.46*cos(rad)));
#else
			c[i] = (float) sh[i];
#endif

	for(;i<nfft+2;i++)
		c[i]= (float)0.0;	/* null rest of memory for fft */

/*
 *  First calculate zero-crossings and filter bank.
 *  Do zero-crossing detection in time domain using first differences
 */

	d1 = c[1] - c[0];
	for(j=0, zc=0; j<windowSize-1; j++) {
		if((nd1=c[j+1]-c[j])*d1 < 0.0)
			zc++;
		d1 = nd1;
	}
	zcd = (float)zc/(float)windowSize;  /* normalize zero crossing to window size */	


/*
 * Other parameters calculated in spectral domain
 */
	ffa(c, nfft);
	for (j=0, k=0; j<nfft/2; j++, k+=2)
		c[j] = (float)sqrt((double)((c[k]*c[k]+c[k+1]*c[k+1]))) / (float) windowSize;

	for(j=0, filt=0.0; j<NFILTERS; j++) {
		m = 1 + (int)((float)(nfft/2) * filtFreqs[j].lo / sampRate);
		n = (int)((float)(nfft/2) * filtFreqs[j].hi / sampRate);
		for(k=m; k<n; k++)
			filt += c[k];
		f[j] = (filt /= (float) (n-m));	/* normalize for bandwidth */
	}

/*
 * Now do comb filter and log results
 */
	combNoLog = 1;  /* don't do and undo log! Results in 1-2% error in comb returned value. */
	combEnergyRatio = combvda((double)(sampRate/2), nfft/2, c, f0, (double)70.0, (double) 400.0);

/*
 * Combine elements for voicing detection
 */
	psum = (float) (zcd * ZCDCOEFF + f[0] * F0COEFF + f[1] * F1COEFF + 
		f[2] * F2COEFF + f[4] * F4COEFF + f[6] * F6COEFF + f[7] * F7COEFF + 
		(float) combEnergyRatio * COMBCOEFF);
	
	psum = (psum-OFFSET) / SCALE;
	psum += 0.5;
	if(psum < 0.0) psum = 0.0;
	if(psum > 1.0) psum = 1.0;

	free(c);
	return psum;
}


/*
 * voicingVerify - 
 *
 * Compare PPS data with independent voicing estimator to flag sampled speech
 * for which voicing was missed (e.g., because EGG contact was lost. 
 * Algorithm: returns the largest number of adjacent frames that were estimated
 * to be 'voiced' but for which PPS was unvoiced. (fail if 4-6 adjacent frames?)
 *
 *		Silvio Eberhardt, 7/99
 */
int voicingVerify(void *samps, long npts, float sampRate, PPSDATA *pps, int isFloat) {
	int j,i,nconsecutive = 0, maxconsecutive=0, voiced, nframes;
	float val, tm;
	float *fl;
	short *sh;
	long windowSize, startPoint;
	double f0;

	windowSize = (int) (WINDOW_LENGTH / 1000.0 * (float)sampRate); // in samples 
	nframes =  (int) floor((double)(npts-(long)windowSize-1L)*(double)FRAME_FREQ/(double)sampRate);
	sh = (short *) samps;
	fl = (float *) samps;

	/* DO EVERY 10 ms using 40 ms windows */
	for(j=0, tm=0.0; j<nframes; j++, tm += ((float)1000.0 / (float)FRAME_FREQ)) {
		startPoint = (long)((float) j * sampRate / (float) FRAME_FREQ);
		if(startPoint + windowSize > npts)	/* exceeded range */
			break;
		if(isFloat)
			val = voicingEstimate(sampRate, (void *)(fl+startPoint), &f0, 1);
		else
			val = voicingEstimate(sampRate, (void *)(sh+startPoint), &f0, 0);

			/* what does EGG have to say? */
		i = pploc(tm, pps);
		voiced = pps->voic[i];
		if(val > THRESHOLD && !voiced)
			nconsecutive++;
		else {
			if(nconsecutive > maxconsecutive)
				maxconsecutive = nconsecutive;
			nconsecutive = 0;
		}
	}
	return maxconsecutive;
}

