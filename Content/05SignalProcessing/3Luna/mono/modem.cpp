////////////////////////////////////////////////////////////////////////////////
//
// modem.cpp 
//
// BaseBand modem simulation                     
//
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

//
// Basic digital communications system blocks:
//
// bytes in  -> FormatSource -> PulseModulate -> BaseBandModulate  -> BandPassModulate     -> XMT
//                             ... AWGN channel ...
// bytes out <- FormateSink  <- Detect        <- BaseBandDemodulate <- BandPassDemodulate  <- RCV
//
#include "FormatSource.h"
#include "PulseModulate.h"
#include "BaseBandModulate.h"
#include "AWGNChannel.h"
#include "BaseBandDemodulate.h"
#include "FormatSink.h"

#include "GnuplotDriver.h"
GnuplotDriver gd;
#define XMITTED_WAVEFORM
//#define PCM_WAVEFORM
#define XMITTED_SIGNAL
#define RECVD_DATA
#define RECVD_SIGNAL

//
// system values.  Passed into subsystems
//
double  pi           = 2.0*atan2(1.0,0.0); 

// M = 2^^k
static const unsigned k    = 2;

// N samples per symbol
static const unsigned N          = 4;

// Lsymbols = -44*R + 33
// where R is nyquist roll-off with range 0.2 < R < 0.75
// from http://www.ee.byu.edu/ee/class/ee485/lectures/raised-cosine.pdf
// pulse span is -Lsymbol/2*Ts <= t <=  Lsymbol/2*Ts)

// corresponds with R = 0.4
static const unsigned Lsymbols   =  8;

// nyquist roll-off
static const double    R          = 0.4;

// PCM voltage level
//
// This voltage level is chosen as USHRT_MAX since we will be
// multiplying it by float values in the Baseband modulation stage.
// This will give us good precision, while leaving lots of room for
// our channel noise.
// 
static const unsigned V          = USHRT_MAX; 

// noise level(sometimes referred to as NSR) corresponds to:
// SNR = Eb/N0 = Eb/noiseLevel,
// when bit energy Eb = 1, then
// SNR = 1/noiseLevel, noiseLevel = 1/SNR
// Frequently SNR is given in dB > 0, so to compute non-dB noiseLevel, just
// perform 1/(10^^SNR) to get noiseLevel.
static double    noiseLevel = 0.90;

static const unsigned bandwidth  = 44100;
static unsigned symbolRate = unsigned(k*44100.0/(2.0*(1.0+R)));
static const double    Tsymbol    = 1.0/symbolRate;

static void PrintHelp(char *s);

static char *prog = "";
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    prog = argv[0];

    // plots some sample filters when true
    bool bPlotBaseBandFilters = false;

    // perform unit tests
    bool bTestUnits = false;
	
    // force transmission of 111111... instead of actual PCM values
    // computed from the data symbols computed from the transmission
    // string.
    bool bTransmitOnes = false;

    // send a single pulse through the system to view the impulse response
    bool bImpResponse = false;

    // transmission string
    char *s = 0L;

    
    while (--argc > 0 && (*++argv)[0] == '-') {
	char c;
	bool bDone = false;
	while (!bDone && (c = *++argv[0]))
	    switch(c) {
	    case 'f':
		bPlotBaseBandFilters = true;
		break;
	    case 'j':
		bTransmitOnes = true;
		break;
	    case 'h':
		PrintHelp("");
		return 0;
		break;
	    case 'i':
		bImpResponse = true;
		break;
	    case 'n':
		if (--argc > 0) {
		    char *sNoiseLevel = *++argv;
		    noiseLevel = fabs(atof(sNoiseLevel));
		    printf("Noise level set to: %lf\n", noiseLevel);
		    bDone = true;
		}
		else {
		    PrintHelp("-n option requires numeric noise level argument");
		    return 0;
		}
		break;
	    case 's':
		if (--argc > 0) {
		    s = *++argv;
		    bDone = true;
		}
		else {
		    PrintHelp("-s option requires argument string");
		    return 0;
		}
		break;
	    case 't':
		bTestUnits = true;
		break;
	    }
    }

    // Initialize gnuplot driver for use during rest of program
    gd.Init();
    //gd.GnuplotCommand("set data style lines\n");
    gd.GnuplotCommand("set data style impulses\n");

    FormatSource       fmtSource;
    PulseModulate      pulseModulate;
    BaseBandModulate   baseBandModulate;
    AWGNChannel        channel;
    BaseBandDemodulate baseBandDemodulate;
    FormatSink         fmtSink;

    if (bTestUnits) {
	fmtSource.Test();
	pulseModulate.Test();
	baseBandModulate.Test();
	baseBandDemodulate.Test();
	fmtSink.Test();
	return 0;
    }

    //
    // initialize components
    //
    if (0L == s) s = "0123";

    fmtSource.SetStream(s);
    fmtSource.Setk(k);
    pulseModulate.SetVoltageLevel(V);

    // accelerate symbol rate for baseBandModulator since in it's
    // universe k is always equal to 1.
    baseBandModulate.SetSymbolRate(symbolRate);
    baseBandModulate.SetSamplesPerSymbol(N);
    baseBandModulate.SetNyquistRollOff(R);
    baseBandModulate.SetPulseSymbolSpan(Lsymbols);

    // setup channel
    channel.SetVoltageLevel(V);
    channel.SetNoiseLevel(noiseLevel);

    // accelerate symbol rate for baseBandDemodulator since in it's
    // universe k is always equal to 1.
    // AN!!! N, R, Lsymbols don't need to match above, but this
    // implementation seems to require.  Can this be fixed?
    baseBandDemodulate.SetSymbolRate(k*symbolRate);
    baseBandDemodulate.SetSamplesPerSymbol(N);
    baseBandDemodulate.SetNyquistRollOff(R);
    baseBandDemodulate.SetPulseSymbolSpan(Lsymbols);

    // baseBandModulator symbol span is based on it's symbolRate, not ours.
    if (bPlotBaseBandFilters) {
	gd.GnuplotCommand("set data style lines\n");
	gd.GnuplotCommand("set xlabel \"Symbol Number (t/Ts)\"\n");
	gd.GnuplotCommand("set ylabel \"Amplitude\"\n");

	symbolRate = unsigned(k*44100/(2.0*(1.75)));
	baseBandModulate.SetSymbolRate(symbolRate);
	baseBandModulate.SetSamplesPerSymbol(32);
	baseBandModulate.SetNyquistRollOff(0.75f);
	baseBandModulate.PlotFilter();

	symbolRate = unsigned(k*44100/(2.0*(1.5)));
	baseBandModulate.SetSymbolRate(symbolRate);
	baseBandModulate.SetNyquistRollOff(0.4f);
	baseBandModulate.PlotFilter();

	symbolRate = unsigned(k*44100/(2.0*(1.1)));
	baseBandModulate.SetSymbolRate(symbolRate);
	baseBandModulate.SetNyquistRollOff(0.1f);
	baseBandModulate.PlotFilter();

	gd.PlotStreams();
	return 0;
    }

    unsigned i, j, m;
    int p, pOld = 0, r, g, z;

    printf("Signal to noise ratio(SNR) Eb/N0 = %lfdB(%lf)\n",
	   10.0*log10(1.0/noiseLevel), 1.0/noiseLevel);
    printf("Channel Bandwidth = %u, Max Symbol Rate = %u and Symbol Period = %lf\n",
	   bandwidth, symbolRate, Tsymbol);
    printf("Transmission string:\n%s\n", s);

    // keep track of bits sent vs recv'd to compute error rate
    unsigned xmitCount = 0;
    unsigned recvCount  = 0;
    unsigned char src[65536];
    unsigned char sink[65536];
    memset(src, 0, 1024);
    memset(sink, 0, 1024);

    int   sampleCount   = 0;
    int   txFilterDelay = N*Lsymbols/2;
    int   rxFilterDelay = N*Lsymbols;
    bool  btxDataHere   = false;
    bool  brxDataHere   = false;

    // each iteration corresponds to delta t == Ts, or 1/symbolRate
    for (i=0; i < (bImpResponse ? 1 : fmtSource.GetMessageSymbolCount()); i++) {
	m = fmtSource.GetMessageSymbol(i);
	pulseModulate.SetSymbol(m, 2);

	// each interation corresponds to delta t == Ts/k
	for (j = 0; j < (bImpResponse ? 1 : k); j++) {
	    bTransmitOnes ? p = V :	p = pulseModulate.GetPCMSignal(j);
	    baseBandModulate.SetPCMSignal(p);

	    // cache bits sent
	    if (p > 0) src[xmitCount++] = 1;
	    else  src[xmitCount++] = 0;
			
#ifdef PCM_WAVEFORM
	    if (pOld != p) {
		gd.AppendStreamData("PCM waveform",
				    .01f+sampleCount/float(N), float(pOld));
	    }
	    gd.AppendStreamData("PCM waveform",
				.01f+sampleCount/float(N), float(p));
	    pOld = p;
#endif
#ifdef XMITTED_WAVEFORM
	    if (pOld != p) {
		gd.AppendStreamData("transmitted waveform",
				    .01f+sampleCount/float(N)+txFilterDelay/float(N),
				    float(pOld));
	    }
	    gd.AppendStreamData("transmitted waveform",
				.01f+sampleCount/float(N)+txFilterDelay/float(N),
				float(p));
	    pOld = p;
#endif
			
	    // first transmitted symbol
	    if (sampleCount >= txFilterDelay)	btxDataHere = true;

	    // each interation corresponds to
	    // delta t == Ts/k*samplesPerSymbol
	    for (unsigned n=0; n < N; n++) {
		g = baseBandModulate.GetBaseBandDigitalSignalSample(n);
				
#ifdef XMITTED_SIGNAL
		gd.AppendStreamData("Modulated PCM waveform(transmitted signal)",
				    sampleCount/float(N), float(g));
#endif
		// pass on to channel and then receiver here
		r = channel.AddNoiseToSignal(g);
		//r = g;

#ifdef RECVD_SIGNAL
		{
		    char title[100];
		    sprintf(title,
			    "Channel output(received signal) SNR Eb/N0 = %lfdB",
			    10.0*log10(1.0/noiseLevel));
		    gd.AppendStreamData(title, sampleCount/float(N), float(r));
		}
#endif
		sampleCount++;
		baseBandDemodulate.SetBaseBandDigitalSignalSample(n, r);
	    }

	    // sample received signal
	    z = baseBandDemodulate.GetFilterSum();

	    // first received symbol
	    if (sampleCount >= rxFilterDelay)	brxDataHere = true;
	    if (brxDataHere) {
#ifdef RECVD_DATA
		gd.AppendStreamData("received data",
				    -txFilterDelay/float(N)+
				    .2f+sampleCount/float(N),
				    float(z)/32.0f);
#endif
		if (z > 0) sink[recvCount++] = 1;
		else  sink[recvCount++] = 0;
	    }
	}
    }

    // first transmitted symbol
    if (sampleCount >= txFilterDelay) btxDataHere = true;

    // first received symbol
    if (sampleCount >= rxFilterDelay)	brxDataHere = true;

#ifdef PCM_WAVEFORM
    gd.AppendStreamData("PCM waveform",
			.01f+sampleCount/float(N), float(pOld));
#endif

    // process lagging symbols
    for (j = 0; j < Lsymbols; j++) {
	baseBandModulate.SetPCMSignal(0);
			
	// last received symbol
	if (j == Lsymbols/2) btxDataHere = false;

	// each interation corresponds to
	// delta t == Ts/k*samplesPerSymbol
	for (unsigned n=0; n < N; n++) {
	    g = baseBandModulate.GetBaseBandDigitalSignalSample(n);
	    // pass on to channel and then receiver here
#ifdef XMITTED_SIGNAL
	    gd.AppendStreamData("Modulated PCM waveform(transmitted signal)",
				sampleCount/float(N), float(g));
#endif
	    // pass on to channel and then receiver here
	    r = channel.AddNoiseToSignal(g);
	    //r = g;

#ifdef RECVD_SIGNAL
	    {
		char title[100];
		sprintf(title,
			"Channel output(received signal) SNR Eb/N0 = %lfdB",
			10.0*log10(1.0/noiseLevel));
		gd.AppendStreamData(title, sampleCount/float(N), float(r));
	    }
#endif
	    sampleCount++;
	    baseBandDemodulate.SetBaseBandDigitalSignalSample(n, r);
	}
	// sample received signal
	z = baseBandDemodulate.GetFilterSum();

	// first received symbol
	if (sampleCount >= rxFilterDelay)
	    brxDataHere = true;

	// last  received symbol
	if (j == Lsymbols-1)
	    brxDataHere = false;

	if (brxDataHere) {
#ifdef RECVD_DATA
	    gd.AppendStreamData("received data",
				-txFilterDelay/float(N)+
				.2f+sampleCount/float(N),
				float(z)/32.0f);
#endif
	    if (z > 0) sink[recvCount++] = 1;
	    else  sink[recvCount++] = 0;
	}
    }

#ifdef XMITTED_SIGNAL
    gd.SetStreamDataPlotOptions("Modulated PCM waveform(transmitted signal)",
				"with lines");
#endif
#ifdef XMITTED_WAVEFORM
    gd.SetStreamDataPlotOptions("transmitted waveform", "with lines");
#endif
#ifdef PCM_WAVEFORM
    gd.SetStreamDataPlotOptions("PCM waveform", "with lines");
#endif

#ifdef RECVD_DATA
    gd.SetStreamDataPlotOptions("received data", "with impulses");
#endif
#ifdef RECVD_SIGNAL
    {
	char title[100];
	sprintf(title,
		"Channel output(received signal) SNR Eb/N0 = %lfdB",
		10.0*log10(1.0/noiseLevel));
		
	gd.SetStreamDataPlotOptions(title, "with lines");
    }
#endif
    gd.PlotStreams();
    printf("\n");

    printf("received string:\n");
    // detect symbols
    // each iteration corresponds to delta t == Ts, or 1/symbolRate
    fmtSink.SetSymbols(sink, recvCount, 1);
    for (i=0; i < (bImpResponse ? 1 : fmtSink.GetDataByteCount()); i++) {
	unsigned char b = fmtSink.GetDataByte(i);
	printf("%c", (char)b);
    }
    printf("\n");

    if (xmitCount != recvCount) {
	printf("Error: bits sent (%d) != bits recv'd (%d)!\n",
	       xmitCount, recvCount);
    }
    int correct = 0, incorrect = 0;
    for (i=0; i<xmitCount; i++) {
	if (src[i] == sink[i]) correct++;
	else incorrect++;
    }
    printf("bits sent = %d, correct = %d, incorrect = %d\n",
	   xmitCount, correct, incorrect);
    printf("Error rate = %f\n", incorrect/float(xmitCount));

    return 0;
} // main()

void PrintHelp(char *s) {
    if (s!= 0L) {
	printf("%s\n", s);
    }
    printf(
	   "Usage: %s [-h][-f][-ij[-t][-n num][-s \"transmission string\"]\n"
	   "Where: -f Plot a sample of transmisson filters and exit\n"
	   "       -h Print this help message\n"
	   "       -i Send a single pulse through system (impulse response)\n"
	   "       -j Jams 1111.. through system instead of transmission string\n"
	   "       -n Set noise level N0 for SNR = Eb/N0, use N0 <= 1.0"
	   "       -s Transmits argument string in \"\"s\n"
	   "       -t Perform unit tests and exit\n", prog);
} // PrintHelp()
