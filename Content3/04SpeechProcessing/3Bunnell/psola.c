#include <stdlib.h>
#include <math.h>
#include "pps.h"

/*
 * Function GetPacket extracts a PSOLA packet from an input waveform array
 * The packet is centered at loc, with (p0-1) samples preceding and 
 * following the sample at loc. A raised cosine window is applied to
 * the packet waveform as it is copied from the input waveform.
 */

void GetPacket(short *waveform, long loc, long p0, short *packet)
{
    long lloc, /* left edge of packet in waveform */
		 rloc, /* right edge of packet in waveform */
		 offs, /* offset to guard against buffer underrun */
		 slop = p0 - 1; /* left tail of windowed region */
    int j;
    double rad, rinc;
/*
 * Adjust start pointer to p0 samples before loc
 * 
 */
    lloc = loc - slop; /* The window will have odd length, centered at loc */
    if (lloc >= 0) {   /* Make sure we don't underrun the output buffer */
        offs = 0;      /* No underrun */
    } else {
        offs = -lloc;  /* Number of samples to skip to avoid underrun */
        lloc = 0;
    }

    rinc = 6.28318530718 / (p0 + p0 - 1); /* Radian increment for cosine window */
    rad = rinc / 2.0;					  /* Initial radian value for cosine window */

    rloc = loc + p0;

    for (j=0; j<offs; j++, rad+=rinc)
      *packet++ = 0;
/*
 * Copy and window the data. Note that we do no error checking here.
 * We assume that there is enough data in the input and space in the
 * output arrays. These need to be checked in the calling routine.
 */
    for (j=lloc; j<rloc; j++, rad+=rinc)
      *packet++ = (short) ((double)waveform[j] * (.5 - .5*cos(rad)));

    return;
}

/*
 * Function PutPacket overlap adds an input PSOLA packet to an output
 * waveform array. The packet is centered at loc, with (p0-1) samples
 * preceding and following the sample at loc.
 */

void PutPacket(short *waveform, long loc, long p0, short *packet)
{
    long lloc, /* left edge of packet in waveform */
		 rloc, /* right edge of packet in waveform */
		 offs, /* offset to guard against buffer underrun */
		 slop = p0 - 1; /* left tail of windowed region */
    int j;
/*
 * Adjust start pointer to slop samples before loc
 * 
 */
    lloc = loc - slop; /* The window will have odd length, centered at loc */
    if (lloc >= 0) {   /* Make sure we don't underrun the output buffer */
        offs = 0;      /* No underrun */
    } else {
        offs = -lloc;  /* Number of samples to skip to avoid underrun */
        lloc = 0;
    }

    rloc = loc + p0;   /* Right edge of packet relative to waveform */

    for (j=0; j<offs; j++)
      packet++;
/*
 * Overlap add the data. Note that we do no error checking here.
 * We assume that there is enough data in the input and space in the
 * output arrays. These need to be checked in the calling routine.
 */
    for (j=lloc; j<rloc; j++)
      waveform[j] += *packet++;

    return;
}


/*
 * Function psola - Pitch Synchronous Overlap Add (time-domain implementation)
 *
 * Synopsis int psola(short *output, short *input, long nin, int sr, float scale);
 *
 *				output	- Pointer to buffer that will receive the generated waveform
 *						  as a sequence of 16-bit samples.
 *				input	- Pointer to buffer containing the input waveform.
 *				nin		- Number of sample in the input.
 *				sr		- Integer Sampling rate in samples per second.
 *				scale	- Pitch period scaling factor. Values > 1.0 decrease F0 while
 *						  values < 1.0 increase F0.
 *
 * Description
 *
 *	This is a minimal implementation to illustrate the use of PSOLA for altering
 *	speech pitch. This function simply copies an input waveform buffer to an output
 *	waveform buffer while scaling the F0 per the supplied scaling factor. The duration
 *	of the waveform is unchanged in this process: pitch periods are duplicated or 
 *	skipped as needed to ensure that duration remains approximately unchanged even
 *	though the number of pitch periods my be significantly increased or decreased.
 *
 *	This implementation is extremely simple. The function ptrack is called to
 *	"pitch track" the input (i.e., locate all the pitch periods and equivalently
 *	spaced voiceless epochs). The ptrack function returns its data in a PPSDATA
 *	structure that it allocates. This structure contains two arrays: pps.time[]
 *	a floating point array which specifies the time (in msec) of each epoch 
 *	detected by ptrack; and pps.voice[] an integer voicing flag that is TRUE if
 *	the epoch was voiced (i.e., an actual pitch period) or FALSE if the epoch
 *	was deemed to be unvoiced. An enhancement to the present routine might use 
 *	the voicing information to alter the way packets are handled by applying a
 *	rectangular window for voiceless data to avoid amplitude modulation of fricative 
 *	sounds.
 *
 *	With the PPSDATA, the 'while' loop in psola copies pitch periods from the
 *	input and places them in the output by scaling the spacing between adjacent
 *	epochs. For scaling factors > 1.0, the spacing of epochs in the output is 
 *	spread out relative to the input spacing and the routine occasionally skips
 *	an epoch in the input so that the timing of the input "keeps up with" the
 *	timing of the output. When scaling is < 1.0, the output epochs are more
 *	closely spaced in time than the input epochs and it is occasionally necessary
 *	to duplicate an epoch to avoid progressing too rapidly through the input.
 *	the if (j != lastj) clause avoids an unnecessary call to GetPacket and 
 *	some unnecessary arithmetic when a period needs to be dupicated.
 */

#define MAXPAC 8192  /* This is the longest packet we will handle */

int psola(short *output, short *input, long nin, int sr, float scale)
{
	long j, jsam, ksam, ppl, lastj;
	double spm = sr/1000.0;
	double tout, duration;
	PPSDATA *pps;
	short *packet;

	packet = (short *) malloc(MAXPAC * sizeof(short));
	pps = ptrack(input, nin, (short) sr);	/* locate pitch periods in input */
	j = 0;									/* Initialize counters and pointers */
	lastj = -1;
	tout = pps->time[0];
	ksam = (long) (tout * spm);
	for (j=0; j<ksam; j++)
		output[j] = 0;

	duration = pps->time[pps->npps - 2];

	while (tout < duration) {				/* Loop till the end of utterance */
	    j = pploc((float) tout, pps);		/* Locate corresponding input PP */
		if (j != lastj) {
			jsam = (long) (spm * (pps->time[j] + 0.5)); /*PP location in input */
			if ((ppl = (long) (spm * (pps->time[j+1] - pps->time[j]))) > MAXPAC)
				ppl = MAXPAC;
			GetPacket(input, jsam, ppl, packet);	/* Get the packet of waveform */
			lastj = j;								/* remember where we got it */
		}
	    PutPacket(output, ksam, ppl, packet);	/* Write it to the output */
	    ksam += (long) (scale * ppl + 0.5);		/* Calculate next Output location */
	    tout = (float) ksam / spm;				/* Calculate progress */
	}
	free(packet);
	free_pps(pps);
	return ksam;
}
