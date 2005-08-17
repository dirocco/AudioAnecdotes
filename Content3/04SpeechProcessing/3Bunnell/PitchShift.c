/* --------------------------------------------------------------------------
 * Name		PitchShift - Shift the F0 of an input file by a constant scale factor
 *
 * Synopsis	PitchShift -psc:sf input[.WAV] [output[.WAV]]
 *		Required args:
 *		  input     input waveform file (RIFF or ASEL)
 *		  -psc:ff   Scale factor 0.0 < sc < ~2.0 values
 *					less than 1.0 lower F0, while
 *					values > 1.0 raise F0.
 *
 *		Optional args:
 *		  output    output waveform file (RIFF or ASEL)
 *
 * Description
 *		Copies input to output while applying the pitch shift
 *		determined by -psc, the scaling factor. If
 *		-psc is > 1.0, the output file will be lower in pitch
 *		than the input (e.g., -psc:2.0 corresponds to a factor of 2
 *		reduction). If -psc is < 1.0, the output is raised in pitch
 *		from the input (e.g., -psc:0.5 is a factor of two increase in
 *		pitch).
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

#define __MAIN__	/* Must be defined in the main program for pps.h */
#include "pps.h"

int psola(short *output, short *input, long nin, int sr, float scale);

static CMDARG  cmdargs[] = {
	{"psc", 1, "1.0"},
	{"", 0, ""}
};

char *cmdfiles[3] = {"", "", ""};

int main(int argc, char **argv)
{
    int nout, nin, maxout;
    float srms, psc;
    short *outbuf, *inbuf;
    char wavout[129] = "", wavin[129];
	WVFILE *wfp;
	WDB wdb;
/* 
 * Check for command line args
 */
	unpack(argc, argv, cmdargs, cmdfiles);

	psc = (float) atof(garg(cmdargs, "psc"));

	strcpy(wavin, cmdfiles[0]);
    if (cmdfiles[1] != NULL)
	strcpy(wavout, cmdfiles[1]);

    if (!(wfp = opnwav(wavin, 'r', &wdb))) {
		printf("PitchShift: Error opening input waveform file: %s\n", wavin);
		exit(0);
    }

	nin = lenwav(wfp);

	srms = (float) (wdb.sample_rate / 1000.0);

/*
 * Estimate length of output file and allocate buffer (leave some extra space)
 *  (Note: use calloc to ensure the buffer is initialized with zeroes).
 */
	maxout = (int) (nin * 1.10);
	outbuf = (short *) calloc(maxout, sizeof(short));


	inbuf = (short *) calloc((nin), sizeof(short));
	if (inwav(wfp, nin, inbuf) != nin) {
		printf("TimeScale: Error reading input waveform file: %s\n", wavin);
		exit(0);
	}
	clswav(wfp);

/* 
 * Call the psola function to achieve the shift
 */

	nout = psola(outbuf, inbuf, nin, (int) wdb.sample_rate, psc);

/*
 * Now write the output waveform
 */
    PABLIO_Stream *stream;
    if (*wavout == '\0') {
      OpenAudioStream(&stream, wdb.sample_rate, paInt16, PABLIO_WRITE | PABLIO_MONO);
      WriteAudioStream(stream, outbuf, nout);
      CloseAudioStream(stream);
      exit(0);
    }

    if (!(wfp = opnwav(wavout, 'n', &wdb))) {
		printf("TimeScale: Error opening output waveform file: %s\n", wavout);
		exit(0);
    }

	if (outwav(wfp, nout, outbuf) != nout) {
		printf("TimeScale: Error writing output waveform %s\n", wavout);
		exit(0);
	}
	clswav(wfp);
	exit(1);
}
