////////////////////////////////////////////////////////////////////////////////
//
// modem.cpp 
//
// BaseBand modulator simulation                     
//
////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//
// Basic digital communications system blocks:
//
// bytes in  -> FormatSource -> PulseModulate -> BaseBandModulate  -> BandPassModulate     -> XMT
//                             ... AWGN channel ...
// bytes out <- FormateSink  <- Detect        <- BaseBandDemodulate <- BandPassDemodulate  <- RCV
//

#include "system.h"
#include "FormatSource.h"
#include "PulseModulate.h"
#include "BaseBandModulate.h"

FormatSource       fmtSource;
PulseModulate      pulseModulate;
BaseBandModulate   baseBandModulate;

#include "GnuplotDriver.h"
GnuplotDriver gd;

bool GetNextSample(void *data, DATA_TYPE type);
void SetData(void *data, DATA_TYPE type);
void PrintHelp(char *s);

char *prog = "";
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
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

    fmtSource.Setk(k);
    pulseModulate.SetVoltageLevel(V);

    // accelerate symbol rate for baseBandModulator since in it's
    // universe k is always equal to 1.
    baseBandModulate.SetSymbolRate(k*symbolRate);
    baseBandModulate.SetSamplesPerSymbol(N);
    baseBandModulate.SetNyquistRollOff(R);
    baseBandModulate.SetPulseSymbolSpan(Lsymbols);

    long datum = 0L; // needs to be enough bytes to hold a float
    unsigned char m;
    int p, g;
    bool debug = false;
    while (GetNextSample(&datum, dataType)) {
        SetData((void *)&datum, dataType);
        datum = 0L;

        // each iteration corresponds to delta t == Ts, or 1/symbolRate
        for (unsigned i=0; i < fmtSource.GetMessageSymbolCount(); i++) {
            m = fmtSource.GetMessageSymbol(i);
            pulseModulate.SetSymbol(m, k);

            // each interation corresponds to delta t == Ts/k
            for (unsigned j = 0; j < k; j++) {
                p = pulseModulate.GetPCMSignal(j);
                baseBandModulate.SetPCMSignal(p);
                // each interation corresponds to
                // delta t == Ts/k*samplesPerSymbol
                for (unsigned n = 0; n < N; n++) {
                    g = baseBandModulate.GetBaseBandDigitalSignalSample(n);
                    printf("%d\n", g);
                }
            }
        }
    }

    // process lagging symbols
    for (unsigned i = 0; i < Lsymbols/2; i++) {
        // each interation corresponds to delta t == Ts/k
        for (unsigned j = 0; j < k; j++) {
            baseBandModulate.SetPCMSignal(0);

            // each interation corresponds to
            // delta t == Ts/k*samplesPerSymbol
            for (unsigned n = 0; n < N; n++) {
                g = baseBandModulate.GetBaseBandDigitalSignalSample(n);
                printf("%d\n", g);
            }
        }
    }
    return 0;
} // main()

void PrintHelp(char *s) {
    if (s) {
        printf("%s\n", s);
    }
    printf(
        "Usage: %s [-h][-f | -i | -s]\n"
        "Where: -h Print this help message\n"
        "       -f Demodulate float data\n"
        "       -i Demodulate int data\n"
        "       -s Demodulate short data(default)\n", prog);
} // PrintHelp()

bool GetNextSample(void *data, DATA_TYPE type) {
    char *format;
    if (FLOAT_DATA == type) {
        format = "%f\n";
    }
    else {
        format = "%d\n";
    }
    return EOF != scanf(format, data);
}

void SetData(void *data, DATA_TYPE type) {
    switch(type) {
        case FLOAT_DATA:
            fmtSource.SetDataFloat(*(float *)data);
            break;
        case INT_DATA:
            fmtSource.SetDataInt(*(int*)data);
            break;
        case SHORT_DATA:
            fmtSource.SetDataShort(*(short*)data);
            break;
        default:
            printf("Error: invalid data type!\n");
            break;
    }
} // SetData()
