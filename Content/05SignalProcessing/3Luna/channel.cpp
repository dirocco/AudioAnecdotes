////////////////////////////////////////////////////////////////////////////////
//
// channel.cpp 
//
// BaseBand channel simulation                     
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//
// Basic digital communications system blocks:
//
// bytes in  -> FormatSource -> PulseModulate -> BaseBandModulate  -> BandPassModulate     -> XMT
//                             ... AWGN channel ...
// bytes out <- FormateSink  <- Detect        <- BaseBandDemodulate <- BandPassDemodulate  <- RCV
//
#include "system.h"
#include "AWGNChannel.h"

#include "GnuplotDriver.h"
GnuplotDriver gd;

void PrintHelp(char *s);

char  *prog = "";

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	prog = argv[0];
	noiseLevel = 0.01f;

	while (--argc > 0 && (*++argv)[0] == '-') {
		char c;
		bool bDone = false;
		while (!bDone && (c = *++argv[0]))
			switch(c) {
			case 'h':
				PrintHelp("");
				return 0;
				break;
			case 'n':
				if (--argc > 0) {
					char *sNoiseLevel = *++argv;
					noiseLevel = fabsf(float(atof(sNoiseLevel)));
					fprintf(stderr, "Noise level set to: %f\n", noiseLevel);
					bDone = true;
				}
				else {
					PrintHelp("-n option requires numeric noise level argument");
					return 0;
				}
				break;
			default:
				PrintHelp("error: flag not supported\n");
				return 1;
				break;
			}
	}

	// Initialize gnuplot driver for use during rest of program
	gd.Init();
	gd.GnuplotCommand("set data style lines\n");

	AWGNChannel        channel;

	//
	// initialize channel
	//
	channel.SetVoltageLevel(V);
	channel.SetNoiseLevel(noiseLevel);

	// pass data thru channel
	int g;
	while (EOF != scanf("%d\n", &g)) {
		g = channel.AddNoiseToSignal(g);
		printf("%d\n", g);
	}

	return 0;
} // main()

void PrintHelp(char *s) {
	if (s!= 0L) {
		printf("%s\n", s);
	}
	printf(
		"Usage: %s [-h][-n <noise level>]\n"
		"Where:  -h Print this help message\n"
		"        -n Set noise level N0 for SNR = Eb/N0, use N0 <= 1.0"
		"           Default of 0.01 is intended to produce few transmission errors.", prog);
} // PrintHelp()
