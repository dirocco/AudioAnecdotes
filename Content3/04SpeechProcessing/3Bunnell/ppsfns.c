/*------------------------------------------------------------------------------------------
 * PPSFNS - A library of functions for tracking and manipulating pitch period
 *			data in digitized speech files.
 *
 * Public functions:
 *   PPSDATA *rd_pps(FILE *ppsfp); 
 *		Read data from a .pps file and construct a PPSDATA container for the
 *		pps file contents. Accepts a pointer to an open stream as input and
 *		returns a pointer to the PPSDATA structure.
 *
 *   long wr_pps(FILE *ppsfp, PPSDATA *pps);
 *		Write data from a PPSDATA structure to a .pps file. Accepts a pointer
 *		to and open FILE stream and a pointer to a PPSDATA structure as input.
 *		returns the number of pitch periods written.
 *
 *   long pploc(float time, PPSDATA *pps);
 *		Searches for the pitch period or voiceless epoch in an input PPSDATA
 *		container that has an onset closest to the input time. Returns the
 *		index of the epoch.
 *
 *   void ppsort(PPSDATA *pps);
 *		Sorts the data in a PPSDATA structure to ensure that the onset times
 *		of the epochs are monotonic increasing within the structure arrays.
 *
 *   void ppadd(float time, int voice, PPSDATA *pps);
 *		Adds a new epoch into the input PPSDATA structure. The new epoch
 *		starts at 'time' and has voicing status 'voice'.
 *
 *   void ppdel(float time, PPSDATA *pps);
 *		Deletes the epoch from the input PPSDATA structure that is closest
 *		to 'time'.
 *
 *	 void cut_pps(float begin, float end, PPSDATA *pps);
 *		Deletes all epochs in 'pps' from 'begin' to 'end' (times given in msec)
 *		Epochs that follow the delete region have their times adjusted as
 *		though the deleted region was never present. This is called by
 *		code that edits waveforms and pitch data.
 *
 *   void clr_pps(float begin, float end, PPSDATA *pps);
 *		Deletes all epochs in 'pps' from 'begin' to 'end' (times in msec). No
 *		adjustment is applied to the times of epochs that follow the deleted
 *		epochs.
 *
 *	 void free_pps(PPSDATA *pps);
 *		called to free the storage allocated for a PPSDATA structure.
 *
 *   int track_pps(WAVDATA *wvd, PPSDATA *pps, long seed, long period, long start, long end);
 *		Searches a waveform to locate all probably pitch epochs within the waveform.
 *		The waveform data is passed in a WAVDATA structure and the located epochs are
 *		returned in a PPSDATA structure. For the remaining fuction parameters,
 *		'seed' is the location of an example pitch period that will be used as
 *		a template to seed the search process; 'period' is the length of the seed 
 *		region in msec; 'start' is the initial sample in waveform of the region to
 *		be searched for epochs; and 'end' is the last sample of the region to be
 *		searched for epochs.
 *
 *	 int btrack_pps(WAVDATA *wvd, PPSDATA *pps, long ss, long es);
 *		An interface to track_pps that first attempts to locate a likely
 *		seed region and then called track_pps with the located seed.
 *
 *   PPSDATA *ptrack (short *wav, long total_samples, unsigned short sample_rate);
 *		An interface to btrack_pps that accepts a monaural waveform as a short
 *		array, characterized by it length and sampling rate and returns a
 *		PPSDATA container with the epochs located within the waveform.
 *
 * H. Timothy Bunnell
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include "libcu.h"
#include "pps.h"

#define TRUE 1
#define FALSE 0

#if !defined M_PI
#define M_PI 3.1415927
#endif

short DC = 0;
static VOICING *vData;
int use_vData = FALSE;


PPSDATA *rd_pps(FILE *ppsfp)
{
	char line[128], word[24], dc;
	long npps, mpps;
	int ip;
	PPSDATA *pps;
	
	npps = 0;
    mpps = 100;
	pps = (PPSDATA *) malloc(sizeof(PPSDATA));
	pps->time = (float *) malloc(100L * sizeof(float));
	pps->voic = (int *) malloc(100L * sizeof(int));
	while (fgets(line, 128, ppsfp)) {
		ip = 0;
		dcs(" ", &ip, line, word, &dc);
		pps->time[npps] = (float) atof(word);
		dcs(" ", &ip, line, word, &dc);
		dcs(" ", &ip, line, word, &dc);
		pps->voic[npps] = atoi(word) == 1;
		npps++;
		if (npps == mpps) {
			mpps += 100;
			pps->time = (float *) realloc((char *)pps->time, mpps * (long) sizeof(float));
			pps->voic = (int *) realloc((char *)pps->voic, mpps * (long) sizeof(int));
		}
	}
	pps->npps = npps;
	pps->mpps = mpps;
	return pps;
}

long wr_pps(FILE *ppsfp, PPSDATA *pps)
{
	int v, j;
	float t, f;
	long npps = pps->npps;

    for (j=0; j<npps - 1; j++) {
		t = pps->time[j];    
		if (pps->voic[j]) {
			v = 1;
			f = (float) (1000.0 / (pps->time[j+1] - t));
		} else {
			v = 0;
			f = 0.0;
		}
    	fprintf(ppsfp, "%8.2f%7.2f  %2d\n", t, f, v);
    }
    if (pps->voic[npps-1])
    	v = 1;
    else
    	v = 0;
   	fprintf(ppsfp, "%8.2f%7.2f  %2d\n", pps->time[npps-1], 0.0f, v);
    return pps->npps;
}
	
long pploc(float time, PPSDATA *pps)
{
	float tdmin, td;
	long j, pploc;
	if (pps->npps > 0) {
		tdmin = (float) fabs(time - pps->time[0]);
		pploc = 0;
		for (j=1; j<pps->npps; j++) {
	    	td = (float) fabs(time - pps->time[j]);
	    	if (td < tdmin) {
				tdmin = td;
				pploc = j;
			}
		}
		return pploc;
	}
	return -1;
}

void ppsort(PPSDATA *pps)
{
	float t;
	int v;
    long j,k;
    
	if (pps->npps > 1) {
		for (j=0; j<pps->npps - 1; j++) {
			for (k=j+1; k<pps->npps; k++) {
				if (pps->time[k] < pps->time[j]) {
					t = pps->time[k];
					pps->time[k] = pps->time[j];
					pps->time[j] = t;
					v = pps->voic[k];
					pps->voic[k] = pps->voic[j];
					pps->voic[j] = v;
				}
			}
		}
	}
	return;
}

void ppadd(float time, int voice, PPSDATA *pps)
{
	long j, ipt;

	if (pps->npps == pps->mpps) {
		pps->mpps += 100;
		pps->time = (float *) realloc(pps->time, pps->mpps * sizeof(float));
		pps->voic = (int *) realloc(pps->voic, pps->mpps * sizeof(int));
	}
	for (j=0; j<pps->npps; j++)
		if (time < pps->time[j])
			break;
			
	ipt = j;
	
	if (ipt < pps->npps) {
	   for (j=pps->npps; j>ipt; --j) {
	      pps->time[j] = pps->time[j-1];
	      pps->voic[j] = pps->voic[j-1];
	   }
	}
	pps->time[ipt] = time;
	pps->voic[ipt] = voice;
	pps->npps++;
	return;
}

void ppdel(float time, PPSDATA *pps)
{
	long j, ipt;

	if (pps->npps > 0) {
		ipt = pploc(time, pps);
	
		if (ipt < pps->npps - 1) {
	   		for (j=ipt; j<pps->npps-1; j++) {
	      		pps->time[j] = pps->time[j+1];
	      		pps->voic[j] = pps->voic[j+1];
	   		}
		}
		pps->npps--;
	}
	return;
}

void cut_pps(float bt, float et, PPSDATA *pps)
{
	long j, k;
	float d = et - bt;
	
	for (j=0, k=0; j<pps->npps; j++) {
		if (pps->time[j] < bt) {
			pps->time[k] = pps->time[j];
			pps->voic[k++] = pps->voic[j];
		} else if (pps->time[j] > et) {
			pps->time[k] = pps->time[j] - d;
			pps->voic[k++] = pps->voic[j];
		}
	}
	pps->npps = k;
	return;
}

void clr_pps(float bt, float et, PPSDATA *pps)
{
	long j, k;
	
	for (j=0, k=0; j<pps->npps; j++) {
		if (pps->time[j] < bt) {
			pps->time[k] = pps->time[j];
			pps->voic[k++] = pps->voic[j];
		} else if (pps->time[j] > et) {
			pps->time[k] = pps->time[j];
			pps->voic[k++] = pps->voic[j];
		}
	}
	pps->npps = k;
	return;
}

void pad_pps(float bt, float d, PPSDATA *pps)
{
	long j;
	
	for (j=0; j<pps->npps; j++) {
		if (pps->time[j] >= bt)
			pps->time[j] += d;
	}
	return;
}


void free_pps(PPSDATA *pps)
{
	free(pps->time);
	free(pps->voic);
	free(pps);
}

float compare(long ipt, long nc, short *data, long nwin, float *kernel)
{
	long j;
	float sum;
	
	data += (ipt - nc * (nwin / 2));
	for (sum = 0.0f, j=0; j< nwin; j++, data+=nc)
		sum += *kernel++ * (float) (*data - DC);
	return sum;
}

long tweak(long ipt, long nc, long ic, short *data, long ns)
{
	if ((data[ipt*nc + ic] - DC) >= 0) {
		while ((ipt >= 0) && ((data[ipt*nc + ic] - DC) >= 0)) {ipt--;}
		return ++ipt;
	} else {
		while ((ipt < (ns-1)) && ((data[ipt*nc + ic] - DC) < 0)) {ipt++;}
		return ipt;
	}	
}

short calc_dc(long ic, long nc, long ns, short *data)
{
	long j;
	float sum = 0.0f;

	for (data+=ic, j=0; j<ns-1; j++, data+=nc)   //xiang chang for j<ns to j<ns-1
		sum += *data;
	return (short) (sum / (float) ns + 0.5f);
}
	
int track_pps(WAVDATA *wvd, PPSDATA *pps, long seed, long period, long ss, long es)
{
	int vstat, navg, fpt;
	long j, ipt, ibeg, iend, nwin, minPP, maxPP, loc, lc2, npps=0, ns, nc, ic;
	float time, tlast, max, v, *kernel, *kernelX, tsc, vuv, pavg, pmin, pmax, sr;
	double rad, rx;
	short *data;

	ns = wvd->ns;
	nc = wvd->nc;
	ic = wvd->active;
	tsc = wvd->srms;
	sr = (float) (tsc * 1000.0);
	data = wvd->wavdata;
/*
 * get windowed kernel at seed location
 */
	DC = calc_dc(ic, nc, ns, data);
	seed = tweak(seed, nc, ic, data, ns);
	nwin = (int) (pitcmn.wlen * tsc + 0.5);
	nwin = 2 * (nwin/2) + 1;  /* nwin should be odd */

	if (seed < nwin/2 || seed > (ns - nwin/2)) {
		return -1;
	}
	
	if (!(kernel = (float *) malloc(nwin * sizeof(float))))
		return -1;

	if (!(kernelX = (float *) malloc(nwin * sizeof(float)))){
		free(kernel);
		return -1;
	}
		
	rx = 2.0 * M_PI/ (float) nwin;

	ibeg = (seed * nc + ic) - (nc * nwin/2);

	for (rad=rx/2.0, ipt=ibeg, j=0; j<nwin; j++, ipt+=nc, rad+=rx)
		kernel[j] = (float) (0.5 - 0.5 * cos(rad)) * (float) (data[ipt] - DC);

	for (rad=rx/2.0, j=0; j<nwin; j++, rad+=rx)
		kernelX[j] = (float) ((0.5 - 0.5 * cos(rad)) * -16384.0 * sin(rad));

	minPP = (int) (pitcmn.jitlo * (float)period + 0.5); /* allow for acceptable jitter */
	maxPP = (int) (pitcmn.jithi * (float)period + 0.5);
	tlast = (float) seed / tsc;
	if (seed >= ss && seed <= es) {
		ppadd(tlast, 1, pps); /* store location of seed as a PP */
		npps++;
	}
	ibeg = seed + minPP;
	iend = seed + maxPP;
	ipt = ibeg;

	if (pitcmn.vuv == 0.0)
    	vuv = (float) (0.1 * compare(seed*nc + ic, nc, data, nwin, kernelX) / (double) nwin);
    else
    	vuv = pitcmn.vuv * pitcmn.vuv;

	pavg = (float) period / tsc;
	navg = 1;
	pmin = pmax = pavg;
/* 
 * Search from seed location to end of the input waveform
 */
	while ((iend + nwin/2) <= es) {
		max = compare(ipt*nc + ic, nc, data, nwin, kernel);
		loc = ipt;
		for (ipt=ibeg+1; ipt < iend; ipt++)
			if ((v = compare(ipt*nc+ic, nc, data, nwin, kernel)) > max) {
				max = v;
				loc = ipt;
			}
		if (use_vData) {
			fpt = (int) ((((double)loc / tsc) - vData->StartTime) / vData->StepSize + 0.5);
			if (fpt < 0)
				fpt = 0;
			if (fpt >= vData->nfr)
				fpt = vData->nfr - 1;
		}
		max = compare(loc*nc+ic, nc, data, nwin, kernelX) / (float) nwin; /* for voicing decision */

		if (max > vuv) {
			lc2 = tweak(loc, nc, ic, data, ns);
			if (labs(lc2 - loc) < nwin/4)
				loc = lc2;
			time = (float) loc / tsc;
			if (vstat) {
				v = time - tlast;
				pavg += v;
				navg++;
				if (v < pmin) pmin = v;
				if (v > pmax) pmax = v;
				if (use_vData) {
					minPP = (long) ((pitcmn.jitlo * sr/vData->F0[fpt]) + 0.5);  /* allow -jitlo variation for next period */
					maxPP = (long) ((pitcmn.jithi * sr/vData->F0[fpt]) + 0.5); /* allow +jithi variation for next period */
				} else {
					minPP = (long) ((pitcmn.jitlo * v) * tsc + 0.5);  /* allow - 10% variation for next period */
					maxPP = (long) ((pitcmn.jithi * v) * tsc + 0.5); /* allow + 15% variation for next period */
				}
			} else {
				if (use_vData)
					v = (sr/vData->F0[fpt])/tsc;
				else	
					v = pavg / (float) navg;
				minPP = (long) ((pitcmn.jitlo * v) * tsc + 0.5);  /* allow - 25% variation for next period */
				maxPP = (long) ((pitcmn.jithi * v) * tsc + 0.5); /* allow + 50% variation for next period */
			}
			vstat = 1;
		} else {
			v = pavg / (float) navg;
			time = tlast + v;
			loc = (long) (tsc * time + 0.5f);
			if (loc > ns) {
				loc = ns;
				time = (float) loc / tsc;
				v = time - tlast;
			}
			vstat = 0;
			minPP = (long) ((pitcmn.jitlo * v) * tsc + 0.5);  /* allow - 25% variation for next period */
			maxPP = (long) ((pitcmn.jithi * v) * tsc + 0.5); /* allow + 50% variation for next period */
		}				
		if (loc >= ss) {
			ppadd(time, vstat, pps);
			npps++;
		}
		ibeg = (loc * nc + wvd->active) - (nc * nwin/2);
		if ((ibeg + nwin) < ns)
			if (vstat)
				for (rad=rx/2.0f, ipt=ibeg, j=0; j<nwin; j++, ipt+=nc, rad+=rx)
					kernel[j] = 0.5f * (kernel[j] + ((float) (0.5 - 0.5 * cos(rad)) * (float) (data[ipt] - DC)));
			else
				for (j=0; j<nwin; j++)
					kernel[j] = 0.5f * (kernel[j] + kernelX[j]);
				
		
		ibeg = loc + minPP;
		iend = loc + maxPP;
		ipt = ibeg;
		tlast = time;
	}

	tlast = (float) seed / tsc;
	ibeg = (seed * nc + ic) - (nc * nwin/2);
	for (rad=rx/2.0, ipt=ibeg, j=0; j<nwin; j++, ipt+=nc, rad+=rx)
		kernel[j] = (float) (0.5 - 0.5 * cos(rad)) * (float) (data[ipt] - DC);

	minPP = (int) (pitcmn.jitlo * (float)period + 0.5); /* reset for original period template */
	maxPP = (int) (pitcmn.jithi * (float)period + 0.5);
	ibeg = seed - maxPP;
	iend = seed - minPP;
	ipt = ibeg;
    
	while ((ibeg >= ss) && (ibeg > nwin/2)) {
		max = compare(ipt*nc + ic, nc, data, nwin, kernel);
		loc = ipt;
		for (ipt=ibeg+1; ipt < iend; ipt++)
			if ((v = compare(ipt*nc+ic, nc, data, nwin, kernel)) > max) {
				max = v;
				loc = ipt;
			}
		if (use_vData) {
			fpt = (int) ((((double)loc / tsc) - vData->StartTime) / vData->StepSize + 0.5);
			if (fpt < 0)
				fpt = 0;
			if (fpt >= vData->nfr)
				fpt = vData->nfr - 1;
		}
		max = compare(loc*nc+ic, nc, data, nwin, kernelX) / (float) nwin; /* for voicing decision */
		if (max > vuv) {
			lc2 = tweak(loc, nc, ic, data, ns);
			if (labs(lc2 - loc) < nwin/4)
				loc = lc2;
			time = (float) loc / tsc;
			if (vstat) {
				v = tlast - time;
				pavg += v;
				navg++;
				if (v < pmin) pmin = v;
				if (v > pmax) pmax = v;
				if (use_vData) {
					minPP = (long) ((pitcmn.jitlo * sr/vData->F0[fpt]) + 0.5);  /* allow -jitlo variation for next period */
					maxPP = (long) ((pitcmn.jithi * sr/vData->F0[fpt]) + 0.5); /* allow +jithi variation for next period */
				} else {
					minPP = (long) ((pitcmn.jitlo * v) * tsc + 0.5);  /* allow - 10% variation for next period */
					maxPP = (long) ((pitcmn.jithi * v) * tsc + 0.5); /* allow + 15% variation for next period */
				}
			} else {
				if (use_vData)
					v = (sr/vData->F0[fpt])/tsc;
				else	
					v = pavg / (float) navg;
				minPP = (long) ((pitcmn.jitlo * v) * tsc + 0.5);  /* allow - 25% variation for next period */
				maxPP = (long) ((pitcmn.jithi * v) * tsc + 0.5); /* allow + 50% variation for next period */
			}
			vstat = 1;
		} else {
			v = pavg / (float) navg;
			time = tlast - v;
			loc = (long) (tsc * time + 0.5);
			if (loc < 0) {
				loc = 0;
				time = (float) loc / tsc;
				v = tlast - time;
			}
			vstat = 0;
			minPP = (long) ((pitcmn.jitlo * v) * tsc + 0.5);  /* allow - 25% variation for next period */
			maxPP = (long) ((pitcmn.jithi * v) * tsc + 0.5); /* allow + 50% variation for next period */
		}				
		if (loc >= ss && loc <= es) {
			ppadd(time, vstat, pps);
			npps++;
		}
		ibeg = (loc * nc + ic) - (nc * nwin/2);
		if (ibeg >= 0)
			if (vstat)
				for (rad=rx/2.0f, ipt=ibeg, j=0; j<nwin; j++, ipt+=nc, rad+=rx)
					kernel[j] = (float) (0.5 * (kernel[j] + ((0.5 - 0.5 * cos(rad)) * (double) (data[ipt] - DC))));
			else
				for (j=0; j<nwin; j++)
					kernel[j] = (float) (0.5 * (kernelX[j] + kernel[j]));
		
		ibeg = loc - maxPP;
		iend = loc - minPP;
		ipt = ibeg;
		tlast = time;
	}

	ppsort(pps);
	free(kernel);
	free(kernelX);
	return (int) npps;
}

int btrack_pps(WAVDATA *wvd, PPSDATA *pps, long ss, long es)
{
	long seed, period, ibeg, iend, ipt, ifr, nfr;
	long ns, nc, ic, j, k, l, loc, nwin;
	float tsc, sr, v, dmin;
	float avgF0, tf0, f0tmp[5], f0sft[5];
	double estF0, vth, navg;
	short *data, max, *x;

	ns = wvd->ns;
	nc = wvd->nc;
	ic = wvd->active;
	tsc = wvd->srms;
	data = wvd->wavdata;
	sr = (float) (tsc * 1000.0);

/*
 * Estimate F0 & prob of voicing every 20 msec with 40 msec window
 */
	nwin = (int) (40.0 * tsc + 0.5);
	if ((es - ss) < nwin)
		return -1;
	nfr = (int) ((double)(es - ss) / (20.0 * tsc) + 0.5); /* actual nfr should be 1 less than this */
	vData = (VOICING *) malloc(sizeof(VOICING));
	vData->StartTime = (float) (tsc*(double)ss + 20.0);
	vData->StepSize = 20.0;
	vData->F0 = (float *) malloc(nfr * sizeof(float));
	vData->Pv = (float *) malloc(nfr * sizeof(float));
	x = (short *) malloc(nwin * sizeof(short));
	ipt = ss;
	ifr = 0;
	avgF0 = (float) 0.0;
	navg = 0.0;
	while ((ipt+nwin) < es) {
		for (j=0; j<nwin; j++) {
			x[j] = data[(ipt+j)*nc+ic];
		}
		vData->Pv[ifr] = voicingEstimate(sr, x, &estF0, FALSE);
		avgF0 += (float) estF0 * vData->Pv[ifr] ;
		navg += vData->Pv[ifr];
		vData->F0[ifr++] = (float) estF0;
		ipt += nwin/2;
	}
	vData->nfr = ifr;
	free(x);
	if (navg)
		avgF0 /= (float) navg;
	else {
		free(vData->Pv);
		free(vData->F0);
		free(vData);
		return -1;
	}
/*
 * Do median smoothing of F0 data
 */
	for (k=0; k<4; k++) 
		f0sft[k] = vData->F0[k];

	for (j=4; j<vData->nfr; j++) {
		f0sft[4] = vData->F0[j];
		for (k=0; k<5; k++)
			f0tmp[k] = f0sft[k];
		for (k=0; k<4; k++) {
			for (l=k+1; l<5; l++) {
				if (f0tmp[l] < f0tmp[k]) {
					tf0 = f0tmp[l];
					f0tmp[l] = f0tmp[k];
					f0tmp[k] = tf0;
				}
			}
		}
		vData->F0[j-2] = f0tmp[2];
		for (k=0; k<4; k++)
			f0sft[k] = f0sft[k+1];
	}
/*
 * Estimate seed (Note: NWIN is now redefined for use with PP detector)
 */
	dmin = 1000.0;
	loc = -1;
	vth = 0.5;
	do {
		for (j=1; j<vData->nfr-1; j++) {
			if (vData->Pv[j-1] >= vth && vData->Pv[j] >= vth && vData->Pv[j+1] >= vth) {
				if (fabs(avgF0 - vData->F0[j]) < dmin) {
					dmin = (float) fabs(avgF0 - vData->F0[j]);
					loc = j;
				}
			}
		}
		vth -= 0.05;
	} while (loc < 0 && vth > .25);
	if (loc < 0) {
		free(vData->Pv);
		free(vData->F0);
		free(vData);
		return -1;
	}

	ipt = (int) (((double)loc * 20.0 + vData->StartTime) * tsc);
	nwin = (int) (pitcmn.wlen * tsc + 0.5);
	nwin = 2 * (nwin/2) + 1;  /* nwin should be odd */
	ibeg = ipt - (int) (0.5*sr/avgF0);
	iend = ipt + (int) (0.5*sr/avgF0);
	if (ibeg < ss)
		ibeg = ss;
	if (iend > es)
		iend = es;
	if (iend <= ibeg) {
		free(vData->Pv);
		free(vData->F0);
		free(vData);
		return -1;
	}

	for (max=0, j=loc=ibeg; j<=iend; j++) {
		if (data[j*nc + ic] > max) {
			loc = j;
			max = data[j*nc + ic];
		}
	}

	seed = tweak(loc, nc, ic, data, ns);

	if (seed < nwin/2 || seed > (ns - nwin/2)) {
		return -1;
	}
	
/*
 * Estimate period
 */
	period = (long) (v = (sr / (pitcmn.F0mean = avgF0)));

/*
 * Now call the regular tracking function with this seed value
 */
	use_vData = TRUE;
	ipt = track_pps(wvd, pps, seed, period, ss, es);
	free(vData->F0);
	free(vData->Pv);
	free(vData);
	use_vData = FALSE;
	return ipt;
}
