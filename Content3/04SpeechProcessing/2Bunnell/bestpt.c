#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float *lowpass(int nin, short *rawwav, double cut);

/*
 *  Function IRUN locates runs of like-signed slope starting at IBEG and
 *  returns IEND, the end of the run.  The routine returns 0 on success 
 *  and -1 if it runs out of samples looking for the next slope change.
 */
int irun(int n, float *signal, int *ibeg, int *iend)
{
    int j;
/*
 *  Starting at IBEG (an extremum), locate the next extremum by locating the
 *  next sign change in the signal slope
 */
    j = *ibeg + 1;
    if (j > n) {
		return -1;
    }
    if (signal[j] >= signal[j - 1]) {
		while(signal[j] >= signal[j - 1]) {
			++j;
			if (j > n)
				return -1;
		}
    } else {
		while(signal[j] <= signal[j - 1]) {
			++j;
			if (j > n)
				return -1;
		}
    }
    *iend = j - 1;
    return 0;
}

/* --------------------------------------------------------------------------
 * Name		BestPt - Best cutpoint for Pitch Period onset
 *
 * Synopsis	int BestPt(int np, short *data, float cut)
 *
 * Description
 *
 *	Locates a point that is most likely to be the begining of a
 * 	pitch period within NP samples of speech waveform passed in DATA.
 *      CUT is a lowpass filter cutoff expressed as a proportion
 *      of the Nyquist interval. The function uses a lowpass filtered
 *	copy of the speech waveform to make the determination. The algorithm
 *	treats the waveform as a sequence of runs separated by extrema and
 *	attempts to locate the run that is most likely to fall at the
 *	onset of a pitch period. Three factors are considered in this: 
 *		1. Run amplitude (the absolute difference between the sample
 *		   values at each end of the run);
 *		2. Run slope (Run Amplitude divided by number of samples in
 *		   the run);
 *		3. Prior run context.
 *	Each run is assigned a score based on these three factors, and
 *	the best scoring run is chosen. The function return is then the
 *	sample following the zero-crossing within the run.
 *
 *
 *  H. T. Bunnell
 * --------------------------------------------------------------------------
 */

int BestPt(int n, short *data, float cut)
{
/* 
 * Local variables
 */
    float dmax;
    int j, ipmid, ret_val;
    float d1, d2, dx, *signal;
    int ipleft, ip1, ip2, ip3, ipright;
/*
 * First lowpass the signal (returns a floating point array of length N
 * in signal).
 */
    signal = lowpass(n, data, cut);
/*
 * Now find most likely run. We do so by tracking the magnitude of the
 * difference between successive maxima and minima in the filtered input.
 * This magnitude is weighted by the run slope to favor runs of steep
 * slope, and we add the difference between an adjacent pair of runs to the 
 * second (later) run if the later run is larger. This favors large runs 
 * that follow small runs.
 *
 */
    ip1 = 0;
    ret_val = 0;  /* Default return value is left edge of the window */

    if (!irun(n, signal, &ip1, &ip2)) {
      d1 = signal[ip2] - signal[ip1];
      if (ip2 > ip1)
	d1 = (d1*d1)/(float)(ip2 - ip1);
      else
	d1 = 0.0;
      ipleft = -1;
      dmax = (float)0.0;
      while(!irun(n, signal, &ip2, &ip3)) {
	d2 = signal[ip3] - signal[ip2];
	if (ip3 > ip2)
	  d2 = (d2*d2)/(float)(ip3 - ip2);
	else
	  d2 = 0.0;

	if (d2 > d1)
	  dx = (float) (d2 * 2 - d1);
	else
	  dx = d2;
	
	if (dx > dmax) {
	  ipleft = ip1;
	  ipmid = ip2;
	  ipright = ip3;
	  dmax = dx;
	}
	ip1 = ip2;
	ip2 = ip3;
	d1 = d2;
      }
/*
 * If a valid point was located, find a zero crossing in the chosen run 
 * and return the first sample location after the zero crossing.
 */
      if (ipleft >= 0) {
	if (data[ipleft] < data[ipmid]) {
	  for (j=ipleft; j<ipmid; ++j) {
	    if (data[j] >= 0) {
	      ret_val = j;
	      goto done;
	    }
	  }
	  ret_val = ipmid;
	} else {
	  for (j=ipmid; j<ipright; ++j) {
	    if (data[j] >= 0) {
	      ret_val = j;
	      goto done;
	    }
	  }
	  ret_val = ipright;
	}
      }
    }

done:
    free(signal);
    return ret_val;
}
