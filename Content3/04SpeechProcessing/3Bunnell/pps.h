#ifndef __PPS_H__
#define __PPS_H__
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __EDWCMN__
typedef struct tagWAVDATA {
	long ns;
	float srms;
	long nc;
	int active;
	short *wavdata;
} WAVDATA;
#endif

typedef struct tagPPSDATA {
	long npps;
	long mpps;
	float *time;
	int *voic;
} PPSDATA;

typedef struct tagVOICING {
	int nfr;
	float StartTime;
	float StepSize;
	float *F0;
	float *Pv;
} VOICING;

#ifndef __MAIN__
extern struct {
#else
struct{
#endif
	float wlen;
	float jitlo;
	float jithi;
	float vuv;
	float F0min;
	float F0max;
	float F0mean;
	int reqseed;
	float egg_thresh;
	float egg_win;
	float egg_offset;
}
#ifdef __MAIN__
pitcmn = {
	8.0,
	(float) 0.70,
	(float) 1.30,
	1000.0,
	80.0,
	240.0,
	120.0,
	0,
	(float) 0.15,
	30.0,
	0.5
};
#else
pitcmn;
#endif

PPSDATA *rd_pps(FILE *ppsfp);
long wr_pps(FILE *ppsfp, PPSDATA *pps);
long pploc(float time, PPSDATA *pps);
void ppsort(PPSDATA *pps);
void ppadd(float time, int voice, PPSDATA *pps);
void ppdel(float time, PPSDATA *pps);
void cut_pps(float begin, float end, PPSDATA *pps);
void clr_pps(float begin, float end, PPSDATA *pps);
void free_pps(PPSDATA *pps);
int track_pps(WAVDATA *wvd, PPSDATA *pps, long seed, long period, long start, long end);
int btrack_pps(WAVDATA *wvd, PPSDATA *pps, long ss, long es);
PPSDATA *ptrack (short *wav, long total_samples, unsigned short sample_rate);
float voicingEstimate(float sampRate, void *data, double *f0, int isFloat);


#ifdef __cplusplus
}
#endif
#endif
