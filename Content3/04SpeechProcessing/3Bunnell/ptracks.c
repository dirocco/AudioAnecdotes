#include <stdio.h>
#include <stdlib.h>
#include "wiodef.h"
#include "libcu.h"

#include "pps.h"

WAVDATA *wvdat;

PPSDATA *PPSD;

PPSDATA *ptrack(short *wav, long total_samples, unsigned short sample_rate)
{
	float srms;
	int rtn;

	srms = (float) (sample_rate / 1000.0);
		
	PPSD = (PPSDATA *) malloc(sizeof(PPSDATA));
	PPSD->time = (float *) malloc(100*sizeof(float));
	PPSD->voic = (int *) malloc(100*sizeof(float));
	PPSD->npps = 0;
	PPSD->mpps = 100;
	wvdat = (WAVDATA *) malloc(sizeof(WAVDATA));
	wvdat->ns = total_samples;
	wvdat->srms = srms;
	wvdat->nc = 1;
	wvdat->active = 0;
	wvdat->wavdata = wav;
	rtn = btrack_pps(wvdat, PPSD, 0, total_samples - 1);
	free(wvdat);
	return PPSD;
}