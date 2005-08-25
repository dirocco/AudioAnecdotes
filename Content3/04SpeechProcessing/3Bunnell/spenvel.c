/*----------------------------------------------------------------------
 * Name			spenvel.c - Spectrum Envelop estimation
 *
 * Synopsis		(double) f0 = spenvel(int npt, float *s, ENVPT *e)
 *
 * Description	Accepts an npt point array (s) containing the log magnitude
 *				power spectrum and returns an ENVPT structure representing
 *				the envelop of the harmonic spectrum as a series of <freq,ampl>
 *				coordinates (see spenvel.h for definition of ENVPT). The function
 *				return value is the estimated f0 which determines the harmonic
 *				spacing for the envelop.
 *
 * Method		spenvel calls function combsrch which searches for a comb filter
 *				with a harmonic spacing within a specified range which passes
 *				the largest amount of spectral energy. This function is called
 *				twice: once with a coarse step size (25 Hz) in searching a broad
 *				range (80 - 600 Hz); and secondly with a fine step size (2.5 Hz)
 *				to search a restricted range of f0 values (+- 12.5 Hz) around the
 *				f0 value returned by the coarse search.
 *----------------------------------------------------------------------
 */

#define WINDOWLENGTH 40.0	/* in ms */


#include <math.h>
#include <stdio.h>
#include "dsp.h"
#include "spenvel.h"

double filterEnergy;				/* energy in best comb filter bands */
double totalEnergy, f0atPeak, eRatio, cepSize, cepRatio, secondPeak, secondF0;
int		doEnergy, calcTotal, combNoLog=0;

double spenvel(double fmax, int npt, float *s, ENVPT *e)
{
	int j, k, np, ib, ie;
	double f0, fsc, bestf0;

	doEnergy = 0;
	fsc = fmax / (double) npt;
/*
 * Start with a coarse search of full f0 range
 */
	bestf0 = combsrch(80.0, 600.0, 1.05, fmax, npt, s);
/*
 * Polish the estimate in the region of the best initial guess
 */
	bestf0 = combsrch(bestf0*.975, bestf0*1.025, 1.01, fmax, npt, s);

/*
 * Allocate storage for the ENVPT structure arrays
 */
	np = (int) (fmax / bestf0 + 0.5);
	e->np = np;
	e->ampl = (float *) malloc(np * sizeof(float));
	e->freq = (float *) malloc(np * sizeof(float));

/*
 * Locate spectral maxima in the immediate region of each "tooth" in
 * the comb and assign them to the corresponding harmonic.
 */
	for (f0=bestf0, k=0; f0<fmax; f0+=bestf0, k++) {
		ib = (int) ((f0 - bestf0/2.0) / fsc + 0.5);
		ie = (int) ((f0 + bestf0/2.0) / fsc + 0.5);
		if (ib < 0)
			ib = 0;
		if (ie >= npt)
			ie = npt-1;
		e->freq[k] = (float) f0;
		e->ampl[k] = s[ib];
		for (j=ib+1; j<=ie; j++)
			if (s[j] > e->ampl[k])
				e->ampl[k] = s[j];
	}

	e->np = k;
	return bestf0;
}


double combvda(double fspect, int npt, float *s, double *estF0, double flo, double fhi) {

	int doAnother;
	double fsc, bestf0, f02, energy1;

	doEnergy = calcTotal = 1;
	totalEnergy  = 0.0;
	fsc = fspect / (double) npt;
	filterEnergy = (double) -1000.0;
/*
 * Start with a coarse search of full f0 range
 */
	bestf0 = combsrch(flo, fhi, 1.05, fspect, npt, s);  // 1.05
	if((secondPeak/filterEnergy) > .95) {
		doAnother = 1;
		f02 = secondF0;
	} else doAnother=0;

/*
 * Polish the estimate in the region of the best initial guess
 */
	bestf0 = combsrch(bestf0*.975, bestf0*1.025, 1.01, fspect, npt, s);

	if(doAnother) {
		energy1 = filterEnergy;
		f02 = combsrch(f02*.975, f02*1.025, 1.01, fspect, npt, s);
		if(filterEnergy > energy1)
			bestf0 = f02;
		else 
			filterEnergy = energy1;
	}


	*estF0 = bestf0;
	return (filterEnergy/totalEnergy);
}

/*-------------------------------------------------------------------------------------
 * Name			combsrch - Search for best fitting comb filter
 *
 * Synopsis		(double) f0 = combsrch(
 *					double flo,		// Lowest possible f0
 *					double fh,		// Highest possible f0
 *					double finc,	// f0 increment for search
 *					double fmax,	// Maximum frequency in spectrum
 *					int npt,		// Number of points in the spectrum
 *					float *s		// an npt-point array containing spectrum
 *					);
 *
 * Description	Tries all f0 values from flo to fhi in finc increments, generating
 *				a simple triangular comb filter at the specified f0 value and measuring
 *				the amount of energy passed through the comb. Returns the highest scoring
 *				f0 value. It does not scan the entire spectrum, only the spectrum up to 
 *				4 kHz or fmax whichever is smaller.
 *-------------------------------------------------------------------------------------
 */
double combsrch(double flo, double fhi, double finc,  double fmax, int npt, float *s)
{
	int j, isign, first;
	double f0, fp, fm, fsc, maxsc, ssc, bestf0, wt;
	double df, fx, flim=4000.0, hw, sw1, sw2;
	double curFiltEnergy, weightArea;

	secondPeak = 0.0;
	fsc = fmax / (double) npt;
	if (flim > fmax)
		flim = fmax;
	maxsc = -50.0;
	bestf0 = flo;
	filterEnergy = 0.0;
	eRatio = -100.0;
	for (f0=flo; f0<fhi; f0*=finc) {
		ssc = (double)0.0;
		fp = df = f0/(double)2.0;
		fm = -df/2.0;
		isign = 0;
		hw = (double) 1.0;
		sw1 = sw2 = (double) 0.0;
		fx = (double) 0.0;
		j = 0;
		first = 1;
		curFiltEnergy = 0.0;			/* for accumulating energy in bands */
		weightArea = 0.0;

		while (fp < flim) {
			while (fx <= fp) {
				if (isign)
					wt = -1.0 + 2.0 * (fx - fm) / df;
				else
					wt = 1.0 - 2.0 * (fx - fm) / df;
				ssc += (wt * (double) s[j++]);
				sw2 += wt;
				fx += fsc;
				if(doEnergy && calcTotal && first) {	/* calculate total energy just once */
					if(!combNoLog)
						totalEnergy += ((pow((double)10.0, (double)(s[j-1]/10.0)) - (double)1.0));
					else
						totalEnergy += s[j-1];
				}
				if(doEnergy && !first) {
					if(fx>=fp)
						wt = 1.0;
					else
						wt = (fx-fm)/df;
					if(!isign)
						wt = 1.0 - wt;
					weightArea += wt;
					if(!combNoLog)
						curFiltEnergy += (wt * (pow((double)10.0, (double)(s[j]/10.0)) - 1.0));
					else 
						curFiltEnergy += wt * s[j];

					if(calcTotal) {
						if(!combNoLog)
							totalEnergy += ((pow((double)10.0, (double)(s[j]/10.0)) - (double)1.0));
						else 
							totalEnergy += s[j];
					}
				}
			}
			isign ^= 1;
			fm = fp;
			fp += df;
			if(first)	/* slope of first (falling) segment is half that of the others, */
				ssc /= (double) 2.0;  /* so wt goes from 0 to -2.0; compensate here */
			first = 0;
		}
		calcTotal = 0;	/* stop calculating total power */
		if (ssc > maxsc) {				/* got a better candidate! Keep track of best. */
			maxsc = ssc;
			eRatio = maxsc;
		}
		curFiltEnergy /= (weightArea / (double) (flim/fsc));  /* normalize for filter areas */
		if(curFiltEnergy > filterEnergy) {
			secondPeak = filterEnergy;
			secondF0 = f0atPeak;
			filterEnergy = curFiltEnergy;
			f0atPeak = f0;
		}
#ifdef DEBUGLOG
			printf("%f %f\n",f0, curFiltEnergy);
#endif
	}
#ifdef DEBUGLOG
	printf("Best F0 is %f, energy is %f, second best F0 is %f, energy is %f\n",
				f0atPeak, filterEnergy, secondF0, secondPeak);
#endif
	return f0atPeak;
}


/*-------------------------------------------------------------------------------------
 * Name			cepitch - Cepstrum Pitch estimator
 *
 * Synopsis		(double) f0 = cepitch(
 *					double flo,		// Lowest possible f0
 *					double fh,		// Highest possible f0
 *					double fmax,	// Maximum frequency in spectrum
 *					int npt,		// Number of points in the spectrum
 *					float *s		// an npt-point array containing log spectrum
 *					);
 *
 * Description	Windows the spectrum array "s" (assumed to be a log magnitude spectrum)
 *				and computes its DFT (formally the cepstrum). Then searches the region
 *				of the cepstrum associated with f0 values between flo and fhi. It returns
 *				the f0 value associated with the largest magnitude quefrency in the
 *				searched range.
 *-------------------------------------------------------------------------------------
 */
double cepitch(double flo, double fhi, double fmax, int npt, float *s)
{
	double fsc, f0, v, vr, vi, vmax, r, rx, cepSecond;
	float *c, sm, ym1, y, yp1;
	int nft, j, k, ib, ie;
	double eavg;

	nft = 16;
	while (nft < npt)
		nft += nft;

	c = (float *) malloc((nft + 2) * sizeof(float));
	fsc = (double) npt / fmax;
	ie = (int) (4000.0 * fsc + 0.5);
	if (ie > npt)
		ie = npt;
	rx = 3.1415927 / (double) ie;
	r = 0.0;
	sm = s[0];
	for (j=0; j<ie; j++) {
		c[j]  = (s[j] - sm) * (float) (0.5 + 0.5*cos(r));
		sm = s[j];
	}
	for (j=ie; j<nft+2; j++)
		c[j] = 0.0f;

	for (k=1; k<3; k++) {
		ym1 = c[0];
		y = c[1];
		for (j=1; j<ie; j++) {
			yp1 = c[j+1];
			c[j] = .25f*ym1 + .5f*y + .25f*yp1;
			ym1 = y;
			y = yp1;
		}
	}

	ffa(c, nft);

/*
 *  Search the requested region for the highest peak and return the associated f0.
 *
 *  fsc = npt / fmax / nft
 *  ib = 1.0 / fhi / fsc
 *  ie = 1.0 / flo / fsc
 *  f0 = 1.0 / (i * fsc)
 */
	vmax = cepSecond = 0.0;
	f0 = 0.0;
	fsc = (double) npt / fmax;
	fsc /= (double) nft;
	ib = (int) ((1.0 / fhi) / fsc + 0.5);
	ie = (int) ((1.0 / flo) / fsc + 0.5);
	for (j=ib, eavg=(double)0.0; j<=ie; j++) {
		vr = c[j*2];
		vi = c[j*2+1];
		v = vr*vr + vi*vi;
		eavg += v;
		if (v > vmax) {
			if(vmax > cepSecond)
				cepSecond = vmax;			/* save second-largest peak */
			cepSize = vmax = v;
			f0 = 1.0f / ((float) j * fsc);
		}
	}
	cepRatio = cepSecond / vmax;
	free(c);
	return f0;
}

