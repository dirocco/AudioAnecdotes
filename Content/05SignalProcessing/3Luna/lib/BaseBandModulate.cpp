////////////////////////////////////////////////////////////////////////////////
//
// BaseBandModulate.cpp
//
// BaseBand modulate incoming (PCM) baseband digital signal
// pi(t). Treats pi(t) as a one bit signal(k=1), so if k > 1, multiply
// system symbolRate by k for this subsystem.
//
// Outputs the N samples of the digital baseband waveform gi(t) shaped
// with a root-raised cosine filter to minimize intersymbol interference(ISI).
// Output from this filter is delayed by
// Lsymbols/2 * system symbol period, and when no active pi(t) this filter
// processes zeros.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "BaseBandModulate.h"
#include "GnuplotDriver.h"
extern GnuplotDriver gd;
extern double         pi;

BaseBandModulate::BaseBandModulate() {
    m_N              = 4;
    m_Lsymbols       = 16;
    m_nyquistRollOff = 0.4f; // aka R
    m_symbolRate     = 44100;// 44.1k default
    m_Tsymbol        = 1/double(m_symbolRate);
    m_scaleFactor    = 1.802946;

    m_PCMSymbols = 0L;
    m_baseBandSamples = 0L;
    m_ReAllocDynamicArrays();
} // BaseBandModulate()

BaseBandModulate::~BaseBandModulate() {
    m_FreeDynamicArrays();
} // ~BaseBandModulate()	

// sets current pi(t) value.  
void BaseBandModulate::SetPCMSignal(int p) {
    // shift symbols values forward in time by Tsymbol and insert p 
    m_ShiftSymbols(p);
    m_Modulate();
} // BaseBandModulate::SetPCMSignal()

// gets gi(t)
int BaseBandModulate::GetBaseBandDigitalSignalSample(unsigned n) {
    if (m_baseBandSamples &&n < m_N) {
	return m_baseBandSamples[n];
    }
    else {
	return 0;
    }
} // BaseBandModulate::GetBaseBandDigitalSignalSample()


//
// Set/Get Nyquist Roll-off for the root raised cosine filter
// 0.0 <= r <= 1.0
//
// Note: The following function may be helpful for determining a
// value for Lsymbols:
//   Lsymbols = -44*R + 33 where 0.2 < R < 0.75
// 
void BaseBandModulate::SetNyquistRollOff(double r) {
    if (0.0f <= r && r <= 1.0f) {
	m_nyquistRollOff = r;
	m_scaleFactor = 1.0/m_z(0.0);
    }
} // BaseBandModulate::SetNyquistRollOff()

double BaseBandModulate::GetNyquistRollOff() {
    return m_nyquistRollOff;
} // BaseBandModulate::GetNyquistRollOff()

// symbolRate > 0
void BaseBandModulate::SetSymbolRate(unsigned symbolRate) {
    if (symbolRate > 0) {
	m_symbolRate = symbolRate;
	m_Tsymbol    = 1/double(symbolRate);
	m_scaleFactor = 1.0/m_z(0.0);
    }
} // BaseBandModulate::SetSymbolRate(k*symbolRate)

unsigned BaseBandModulate::GetSymbolRate() {
    return m_symbolRate;
} // BaseBandModulate::GetSymbolRate()

// Lsymbols > 0.  See note on SetNyquistRollOff().
void BaseBandModulate::SetPulseSymbolSpan(unsigned Lsymbols) {
    if (Lsymbols > 0 && m_Lsymbols != Lsymbols) {
	m_Lsymbols = Lsymbols;
	m_ReAllocDynamicArrays();
	m_scaleFactor = 1.0/m_z(0.0);
    }
} // BaseBandModulate::SetPulseSymbolSpan(Lsymbol)

unsigned BaseBandModulate::GetPulseSymbolSpan() {
    return m_Lsymbols;
} // BaseBandModulate::GetPulseSymbolSpan()

// N > 0
void BaseBandModulate::SetSamplesPerSymbol(unsigned N) {
    if (N > 0 && m_N != N) {
	m_N = N;
	m_ReAllocDynamicArrays();
	m_scaleFactor = 1.0/m_z(0.0);
    }
} // BaseBandModulate::SetSamplesPerSymbol(N)

unsigned BaseBandModulate::GetSamplesPerSymbol() {
    return m_N;
} // BaseBandModulate::GetSamplesPerSymbol()

void BaseBandModulate::PlotFilter() {
    char title[256];
    double deltat = m_Tsymbol/m_N;
    double halfspan = m_Lsymbols/2.0f;
    double t0;
    unsigned i, j;

    sprintf(title,
	    "raised cosine xmit filter: N=%d, Lsymbols=%d, R=%f",
	    m_N, m_Lsymbols, m_nyquistRollOff);
    for (i=0; i < m_Lsymbols; i++) {
	t0 = (i-halfspan)*m_Tsymbol;
	for (j=0; j < m_N; j++) {
	    gd.AppendStreamData(title,
				float((t0+j*deltat)/m_Tsymbol),
				float(m_scaleFactor*m_z(t0+j*deltat)));
	}
    }
    //gd.SetStreamDataPlotOptions(title, "with impulses");
} // BaseBandModulate::PlotFilter()

void BaseBandModulate::Test() {
    printf("BaseBandModulate::Test() NYI!!\n");
} // BaseBandModulate::Test()

// essentially a FIFO array en-queuing. Drops shifted off value.
void BaseBandModulate::m_ShiftSymbols(int value) {
    if (0L == m_PCMSymbols) {
	fprintf(stderr,
		"BaseBandModulate::m_ShiftSymbols() error: null queue!\n");
	return;
    }
    for (unsigned i = m_Lsymbols-1; i > 0; i--) {
	m_PCMSymbols[i] = m_PCMSymbols[i-1];
    }
    m_PCMSymbols[0] = value;
} // BaseBandModulate::m_ShiftSymbols(int value)

//
// compute baseband digital signal output samples(gi(t)) from (m_Lsymbols)
// span of input PCM symbols modulated with z(t)
//
void BaseBandModulate::m_Modulate() {
    double deltat = m_Tsymbol/m_N;
    double halfspan = m_Lsymbols/2.0f;
    double t0;
    unsigned i, j;
	
    // zero out output signal samples
    memset(m_baseBandSamples, 0, m_N*sizeof(int));
    for (i=0; i < m_Lsymbols; i++) {
	t0 = (i-halfspan)*m_Tsymbol;
	for (j=0; j < m_N; j++) {
	    m_baseBandSamples[j] += int(m_PCMSymbols[i]*m_scaleFactor*m_z(t0+j*deltat));
	}
    }
} // BaseBandModulate::m_Modulate()

//
// nyquist root raised cosine pulse centered at t=0
//
double BaseBandModulate::m_z(double t) {
    double alpha   = 2*m_nyquistRollOff/pi;
    double beta    = 1-t*t*16*m_nyquistRollOff*m_nyquistRollOff/(m_Tsymbol*m_Tsymbol);
    double gamma   = (1-m_nyquistRollOff)*pi/m_Tsymbol;
    double delta   = (1+m_nyquistRollOff)*pi/m_Tsymbol;
    double epsilon;

    // substitute limit value for sin(gamma*t)/(2*pi*t) singularity
    if (0.0f != t) {
	epsilon = m_Tsymbol*sin(gamma*t)/(2*t*pi);
    }
    else {
	epsilon = m_Tsymbol*(gamma/(2*pi));
    }

    // substitute limit value for alpha*cos(delta*t)/beta singularity
    if (0.0 == beta) {
	return alpha;
    }
    return double((epsilon+alpha*cos(delta*t))/beta);
} // double BaseBandModulate::m_z(double t)

void BaseBandModulate::m_ReAllocDynamicArrays() {
    m_FreeDynamicArrays();
    m_PCMSymbols      = new int[m_Lsymbols];
    if (0L == m_PCMSymbols) {
	fprintf(stderr,	"BaseBandModulate:: error: memory allocation failed!\n");
	return;
    }
    memset(m_PCMSymbols, 0, m_Lsymbols*sizeof(int));

    m_baseBandSamples = new int[m_N];
    if (0L == m_baseBandSamples) {
	fprintf(stderr,	"BaseBandModulate:: error: memory allocation failed!\n");
	return;
    }
    memset(m_baseBandSamples, 0, m_N*sizeof(int));
} // BaseBandModulate::m_ReAllocDynamicArrays()

void BaseBandModulate::m_FreeDynamicArrays() {
    if (0L != m_PCMSymbols) {
	if (1 >= m_Lsymbols) {
	    delete m_PCMSymbols;
	}
	else {
	    delete [] m_PCMSymbols;
	}
	m_PCMSymbols = 0L;
    }
    if (0L != m_baseBandSamples) {
	if (1 >= m_N) {
	    delete m_baseBandSamples;
	}
	else {
	    delete [] m_baseBandSamples;
	}
	m_baseBandSamples = 0L;
    }
} // BaseBandModulate::m_FreeDynamicArrays()

