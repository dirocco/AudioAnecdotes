////////////////////////////////////////////////////////////////////////////////
//
// demodulator.cpp 
//
// BaseBand demodulator simulation                     
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

//
// Basic digital communications system blocks:
//
// bytes in  -> FormatSource -> PulseModulate -> BaseBandModulate   -> BandPassModulate    -> XMT
//                             ... AWGN channel ...
// bytes out <- FormateSink  <- Detect        <- BaseBandDemodulate <- BandPassDemodulate  <- RCV
//

#include "system.h"
#include "BaseBandDemodulate.h"
#include "FormatSink.h"

#include "GnuplotDriver.h"
GnuplotDriver gd;

BaseBandDemodulate baseBandDemodulate;
FormatSink         fmtSink;

char *prog = "";
void PrintHelp(char *s);
void PrintData(unsigned symbolsCnt, unsigned char *symbols, DATA_TYPE type);
bool bPlotData = false;
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    const unsigned samples = sizeof(float)*8;

    unsigned char m[samples];  // max data size * 8 bits
    unsigned i, j, n;
    int r, z;

    DATA_TYPE dataType = SHORT_DATA;

    prog = argv[0];
    while (--argc > 0 && (*++argv)[0] == '-') {
	char c;
	while (c = *++argv[0])
	    switch(c) {
	    case 'f':
		dataType = FLOAT_DATA;
		break;
	    case 'i':
		dataType = INT_DATA;
		break;
	    case 's':
		dataType = SHORT_DATA;
		break;
	    case 'p':
		bPlotData = true;
		break;
	    case 'h':
		PrintHelp("");
		return 0;
		break;
	    default:
		PrintHelp("error: flag not supported\n");
		return 1;
		break;
	    }
    }

    //
    // initialize components
    //

    // accelerate symbol rate for baseBandDemodulator since in it's
    // universe k is always equal to 1.
    baseBandDemodulate.SetSymbolRate(k*symbolRate);
    baseBandDemodulate.SetSamplesPerSymbol(N);
    baseBandDemodulate.SetNyquistRollOff(R);
    baseBandDemodulate.SetPulseSymbolSpan(Lsymbols);

    if (bPlotData) {
	gd.Init();
    }
    // process preceding symbols
    for (j = 0; j < Lsymbols/2+1; j++) {
	for (n = 0; n < N; n++) {
	    if (scanf("%d\n", &r) == EOF) {
		return 1;
	    }
	    baseBandDemodulate.SetBaseBandDigitalSignalSample(n, r);
	}
	z = baseBandDemodulate.GetFilterSum();
    }

    i = 0; n = 0;
    while (EOF != scanf("%d\n", &r)) {
	baseBandDemodulate.SetBaseBandDigitalSignalSample(n, r);
	if (++n == N) {
	    n = 0;
	    // correlate received signal samples and apply decision criteria.
	    // This also demodulates the PCM applied in the modulator.
	    m[i] = ((z = baseBandDemodulate.GetFilterSum()) > 0) ? 1 : 0;
	    if (bPlotData) {
		gd.AppendStreamData("Receiver output", float(z));
	    }
	    if (!(i = ++i % samples)) {
		PrintData(samples, m, dataType);
	    }
	}
    }
    if (--i > 0) {
	PrintData(i, m, dataType);
    }

    if (bPlotData) {
	gd.PlotStreams();
    }

    return 0;
} // main()

void PrintHelp(char *s) {
    if (s!= 0L) {
	printf("%s\n", s);
    }
    printf(
	   "Usage: %s [-h][-f | -i | -s]\n"
	   "Where: -h Print this help message\n"
	   "       -f Demodulate float data\n"
	   "       -i Demodulate int data\n"
	   "       -s Demodulate short data(default)\n", prog);
} // PrintHelp()

void PrintDataFloat(unsigned symbolsCnt, unsigned char *symbols);
void PrintDataInt(unsigned symbolsCnt, unsigned char *symbols);
void PrintDataShort(unsigned symbolsCnt, unsigned char *symbols);

void PrintData(unsigned symbolsCnt, unsigned char *symbols, DATA_TYPE type) {
    // quietly ignore lack of data
    if (symbolsCnt > 0) {
	switch(type) {
	case FLOAT_DATA:
	    PrintDataFloat(symbolsCnt, symbols);
	    break;
	case INT_DATA:
	    PrintDataInt(symbolsCnt, symbols);
	    break;
	case SHORT_DATA:
	    PrintDataShort(symbolsCnt, symbols);
	    break;
	default:
	    printf("Error: invalid data type!\n");
	    break;
	}
    }
} // PrintData()

void PrintDataShort(unsigned symbolsCnt, unsigned char *symbols) {
    short b;
    fmtSink.SetSymbols(symbols, symbolsCnt, 1);
    for (unsigned i = 0; i < fmtSink.GetDataShortCount(); i++) {
	b = fmtSink.GetDataShort(i);
	printf("%d\n", b);
    }
} // PrintDataShort()

void PrintDataInt(unsigned symbolsCnt, unsigned char *symbols) {
    int b;
    fmtSink.SetSymbols(symbols, symbolsCnt, 1);
    for (unsigned i = 0; i < fmtSink.GetDataIntCount(); i++) {
	b = fmtSink.GetDataInt(i);
	printf("%d\n", b);
    }
} // PrintDataInt()

void PrintDataFloat(unsigned symbolsCnt, unsigned char *symbols) {
    float b;
    fmtSink.SetSymbols(symbols, symbolsCnt, 1);
    for (unsigned i = 0; i < fmtSink.GetDataFloatCount(); i++) {
	b = fmtSink.GetDataFloat(i);
	printf("%13.13e\n", b);
    }
} // PrintDataFloat()
