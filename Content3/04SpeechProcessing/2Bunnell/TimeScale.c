/* --------------------------------------------------------------------------
 * Name		TimeScale - Compress or Expand the time-scale of a waveform
 *
 * Synopsis	TimeScale -tsc:ff [-opt:par] input[.WAV] [output[.WAV]]
 *		Required args:
 *		  input     input waveform file (RIFF or ASEL)
 *		  -tsc:ff   Scale factor 0.0 < sc < MAX_FLOAT values
 *			    less than 1.0 produce compression, while
 *			    values > 1.0 produce expansion.
 *
 * 		Optional args:
 *		  output    output waveform file (RIFF or ASEL)
 * 		  -min:mm   Minimum pitch period length in msec [4]
 *		  -max:mm   Maximum pitch period length in msec [12]
 * 		  -lap:nn   Number of overlapping samples for gating from 
 *			    one period to the next [32]
 *
 * Description
 *		Copies input to output while applying the time compression
 *		or expansion determined by -tsc, the scaling factor. If
 *		-tsc is < 1.0, the output file will be a compressed version
 *		of the input (e.g., -tsc:0.5 corresponds to a factor of 2
 *		compression). If -tsc is > 1.0, the output is expanded from
 *		the input (e.g., -tsc:2.0 is a factor of two expansion).
 *
 * --------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pablio.h>
#include "wiodef.h"
#include "libcu.h"
#include "dsp.h"

static CMDARG  cmdargs[] = {
	{"tsc", 1, "1.0"},
	{"min", 1, "4.0"},
	{"max", 1, "12.0"},
	{"lap", 1, "32"},
	{"", 0, ""}
};

char *cmdfiles[3] = {"", "", ""};

int BestPt(int n, short *data, float cut);

void Usage() {
  printf("Usage: TimeScale -tsc:ff [-opt:par] <input[.wav]> <output[.wav]>\n");
  printf("       -tsc:ff -- scale factor 0.0 < ff < MAX_FLOAT. If ff < 1.0\n");
  printf("                  time scale is compressed. If ff > 1.0 time scale\n");
  printf("                  is expanded\n");
  printf("       -min:ms -- Minimum pitch period length in msec\n");
  printf("       -max:ms -- Maximum pitch period length in msec\n");
  printf("       -lap:ns -- Number of overlap samples to minimize transients\n");
  exit(1);
}

int main(int argc, char **argv)
{
    int lap, nout, j, nwork, nin;
    int isamp, jsamp, ksamp, lsamp;
    int maxst, minst, nsamp, maxout;
    float pinc, fmax, srms, p, duration;
    float stmax, stmin;
    float wl[128], wr[128], timein, timeot, tsc, cut;
    short *outbuf, *inbuf;
    char wavout[129] = "", wavin[129];
    WVFILE *wfp;
    WDB wdb;

/*
 * Trap bad command
 */
    if (argc < 3)
      Usage();

/* 
 * Check for command line args
 */
    unpack(argc, argv, cmdargs, cmdfiles);

    tsc = (float) atof(garg(cmdargs, "tsc"));
    lap = atoi(garg(cmdargs, "lap"));
    stmax = (float) atof(garg(cmdargs, "max"));
    stmin = (float) atof(garg(cmdargs, "min"));
    
    strcpy(wavin, cmdfiles[0]);
    if (cmdfiles[1] != NULL)
    strcpy(wavout, cmdfiles[1]);

    if (!(wfp = opnwav(wavin, 'r', &wdb))) {
      printf("TimeScale: Error opening input waveform file: %s\n", wavin);
      exit(0);
    }

    nin = lenwav(wfp);

    srms = (float) (wdb.sample_rate / 1000.0);
    fmax = (float) (wdb.sample_rate / 2.0);
    cut = (float)(1200.0 / fmax);
    maxst = (int) (srms * stmax + 0.5);
    minst = (int) (srms * stmin + 0.5);

/*
 * Estimate length of output file and allocate buffer (leave some extra space)
 *  (Note: use calloc to ensure the buffer is initialized with zeroes).
 */
    maxout = (int) (tsc * nin + 2 * maxst + 0.5);
    outbuf = (short *) calloc(maxout, sizeof(short));

    if (lap > 128) {
      printf("TimeScale: Using maximum lap of 128 samples.\n");
      lap = 128;
    }

/*
 * If we are using overlap blending, compute the blending weights
 * Probably linear weights would be fine, but we use cosine weights.
 */
    if (lap > 0) {
      p = pinc = (float) (3.1415926 / (lap+1));
      for (j=0; j < lap; ++j) {
	wl[j] = (float) (0.5 + 0.5*cos(p));
	wr[j] = (float) (1.0 - wl[j]);
	p += pinc;
	outbuf[j] = 0;
      }
    }

/*
 * Initialization: time pointers and parameters. The working buffer is
 * actually just a region within the input buffer.
 */
    timeot = timein = 0.0;
    nwork = lap + (maxst * 2) + minst;
    duration = (float) ((nin - nwork) / srms); /* length - working window */

    if (duration < stmax) {
      printf("TimeScale: Input is too short to process.\n");
      exit(0);
    }

/*
 * We allocate enough space in the input buffer for lap points before and
 * maxst points after the end of the actual waveform data. The input data
 * is offset by lap points initially and the first epoch is defined to 
 * begin at original waveform sample 0.
 */
    inbuf = (short *) calloc((lap + nin + maxst), sizeof(short));
    if (inwav(wfp, nin, inbuf+lap) != nin) {
      printf("TimeScale: Error reading input waveform file: %s\n", wavin);
      exit(0);
    }
    clswav(wfp);

/* 
 * Loop from here until we reach the end of the input
 */
    lsamp = 0;
    while(timein < duration) {
/*
 * Convert timein to a sample pointer. Adjust the sample pointer isamp
 * to center timein within a working window of nwork samples.
 */
      isamp = (int) (timein * srms + 0.5);

/*
 * locate the probable start of a pitch period within the working window
 */
      if (isamp > 0)
	jsamp = isamp + lap + BestPt(maxst, inbuf+isamp+lap, cut);
      else
	jsamp = 0;
/*
 * locate probable end of the pitch period (i.e., probable start of the
 * next pitch period and compute pitch period length in samples (nsamp)
 * Note that ksamp points to the start of the NEXT PP so 
 * nsamp = ksamp - jsamp
 */
      ksamp = jsamp + minst + BestPt(maxst, inbuf+jsamp+minst, cut);
      nsamp = ksamp - jsamp;
/*
 * Do overlap blending if a lap interval was specified
 */
      if (lap && (jsamp > lap)) {
	for (j = 0; j < lap; ++j) {
	  outbuf[lsamp+j] = (short) (outbuf[lsamp+j]*wl[j] + inbuf[jsamp-lap+j]*wr[j]);
	}
      }
/*
 * Copy this chunk to the output buffer.
 */
      for (j = 0; j < nsamp; ++j) {
	outbuf[lsamp+j+lap] = inbuf[jsamp+j];
      }
/*
 * Update timeot and scale it using tsc to find new timein.
 */
      lsamp += nsamp;
      timeot = (float) ((lsamp-lap) / srms);
      timein = timeot / tsc;
    }

/*
 * Now write the output waveform
 */
    PABLIO_Stream *stream;
    if (*wavout == '\0') {
      OpenAudioStream(&stream, wdb.sample_rate, paInt16, PABLIO_WRITE | PABLIO_MONO);
      nout = lsamp - 1;
      WriteAudioStream(stream, outbuf, nout);
      CloseAudioStream(stream);
      exit(0);
    }

    if (!(wfp = opnwav(wavout, 'n', &wdb))) {
      printf("TimeScale: Error opening output waveform file: %s\n", wavout);
      exit(0);
    }
    nout = lsamp - 1;
    if (outwav(wfp, nout, outbuf+lap) != nout) {
      printf("TimeScale: Error writing output waveform %s\n", wavout);
      exit(0);
    }
    clswav(wfp);
    exit(1);
}
