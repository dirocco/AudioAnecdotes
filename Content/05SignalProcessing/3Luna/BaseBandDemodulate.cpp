////////////////////////////////////////////////////////////////////////////////
//
// BaseBandDemodulate.cpp
//
// BaseBand demodulate incoming baseband digital signal (r(t)) samples
// by application of a matched filter; in this case a root raised
// cosine pulse.  r(t) is treated as a one bit signal(k=1) sampled N
// times, so if k > 1, multiply system symbolRate by k for this
// subsystem.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "BaseBandDemodulate.h"
#include "GnuplotDriver.h"
extern GnuplotDriver gd;
extern double         pi;

BaseBandDemodulate::BaseBandDemodulate() {
    m_N              = 4;
    m_Lsymbols       = 16;
    m_nyquistRollOff = 0.4f; // aka R
    m_symbolRate     = 44100;// 44.1kHz default
    m_Tsymbol        = 1/double(m_symbolRate);
    m_scaleFactor    = 1.802946;

    m_Z               = 0L;
    m_baseBandSamples = 0L;
    m_ReAllocDynamicArrays();
} // BaseBandDemodulate()

BaseBandDemodulate::~BaseBandDemodulate() {
    m_FreeDynamicArrays();
} // ~BaseBandDemodulate()	

// gets filter sum, Z(T)
int BaseBandDemodulate::GetFilterSum() {
    m_ShiftMatchedFilterOutput(0);
    m_Demodulate();

    if (0L != m_Z) {
	return m_Z[m_Lsymbols-1];
    }
    else {
	printf("GetFilterSum error: no filter sum!\n");
	return 0;
    }
} // BaseBandDemodulate::GetFilterSum()

// sets r(t)
void BaseBandDemodulate::SetBaseBandDigitalSignalSample(unsigned n, int sample)
{
    if (0L == m_baseBandSamples) {
	fprintf(stderr,
		"SetBaseBandDigitalSignalSample: null sample storage!\n");
    }
    else if (n >= m_N) {
	fprintf(stderr,
		"SetBaseBandDigitalSignalSample: %d outside 0 <= n < %d!\n",
		n, m_N);
    }
    else {
	m_baseBandSamples[n] = sample;
    }
} // BaseBandDemodulate::SetBaseBandDigitalSignalSample()

//
// Set/Get Nyquist Roll-off for the root raised cosine filter
// 0.0 <= r <= 1.0
//
// Note: The following function may be helpful for determining a
// value for Lsymbols:
//   Lsymbols = -44*R + 33 where 0.2 < R < 0.75
// 
void BaseBandDemodulate::SetNyquistRollOff(double r) {
    if (0.0f <= r && r <= 1.0f) {
	m_nyquistRollOff = r;
	m_scaleFactor = 1.0/m_z(0.0);
    }
} // BaseBandDemodulate::SetNyquistRollOff()

double BaseBandDemodulate::GetNyquistRollOff() {
    return m_nyquistRollOff;
} // BaseBandDemodulate::GetNyquistRollOff()

// symbolRate > 0
void BaseBandDemodulate::SetSymbolRate(unsigned symbolRate) {
    if (symbolRate > 0) {
	m_symbolRate = symbolRate;
	m_Tsymbol    = 1/double(symbolRate);
	m_scaleFactor = 1.0/m_z(0.0);
    }
} // BaseBandDemodulate::SetSymbolRate(k*symbolRate)

unsigned BaseBandDemodulate::GetSymbolRate() {
    return m_symbolRate;
} // BaseBandDemodulate::GetSymbolRate()

// Lsymbols > 0.  See note on SetNyquistRollOff().
void BaseBandDemodulate::SetPulseSymbolSpan(unsigned Lsymbols) {
    if (Lsymbols > 0 && m_Lsymbols != Lsymbols) {
	m_Lsymbols = Lsymbols;
	m_ReAllocDynamicArrays();
	m_scaleFactor = 1.0/m_z(0.0);
    }
} // BaseBandDemodulate::SetPulseSymbolSpan(Lsymbol)

unsigned BaseBandDemodulate::GetPulseSymbolSpan() {
    return m_Lsymbols;
} // BaseBandDemodulate::GetPulseSymbolSpan()

// N > 0
void BaseBandDemodulate::SetSamplesPerSymbol(unsigned N) {
    if (N > 0 && m_N != N) {
	m_N = N;
	m_ReAllocDynamicArrays();
	m_scaleFactor = 1.0/m_z(0.0);
    }
} // BaseBandDemodulate::SetSamplesPerSymbol(N)

unsigned BaseBandDemodulate::GetSamplesPerSymbol() {
    return m_N;
} // BaseBandDemodulate::GetSamplesPerSymbol()

void BaseBandDemodulate::PlotFilter() {
    char title[256];
    double deltat = m_Tsymbol/m_N;
    double halfspan = m_Lsymbols/2.0f;
    double t0;
    unsigned i, j;

    sprintf(title,
	    "raised cosine recv filter: N=%d, Lsymbols=%d, R=%f",
	    m_N, m_Lsymbols, m_nyquistRollOff);
    for (i=0; i < m_Lsymbols; i++) {
	t0 = (i-halfspan)*m_Tsymbol;
	for (j=0; j < m_N; j++) {
	    gd.AppendStreamData(title,
				float((t0+j*deltat)/m_Tsymbol),
				float(m_scaleFactor*m_z(t0+j*deltat)));
	}
    }
    gd.SetStreamDataPlotOptions(title, "with impulses");
} // BaseBandDemodulate::PlotFilter()

void BaseBandDemodulate::Test() {
    printf("BaseBandDemodulate::Test() NYI!!\n");
} // BaseBandDemodulate::Test()

// essentially a FIFO array en-queuing. Drops shifted off value.
void BaseBandDemodulate::m_ShiftMatchedFilterOutput(int value) {
    if (0L == m_Z) {
	fprintf(stderr,
		"BaseBandDemodulate::m_ShiftMatchedFilterOutput() error: null queue!\n");
	return;
    }
    for (unsigned i = m_Lsymbols-1; i > 0; i--) {
	m_Z[i] = m_Z[i-1];
    }
    m_Z[0] = value;
} // BaseBandDemodulate::m_ShiftMatchedFilterOutput(int value)

//
// BaseBand demodulate incoming baseband digital signal (r(t)) samples
// by application of a matched filter.
//
void BaseBandDemodulate::m_Demodulate() {
    double deltat = m_Tsymbol/m_N;
    double halfspan = m_Lsymbols/2.0f;
    double t0;
    unsigned i, j;
	
    for (i=0; i < m_Lsymbols; i++) {
	t0 = (i-halfspan)*m_Tsymbol;
	for (j=0; j < m_N; j++) {
	    m_Z[i] +=
		int(double(m_baseBandSamples[j])*m_scaleFactor*m_z(t0+j*deltat));
	}
    }
} // BaseBandDemodulate::m_Modulate()

//
// nyquist root raised cosine pulse centered at t=0
//
double BaseBandDemodulate::m_z(double t) {
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
} // double BaseBandDemodulate::m_z(double t)

void BaseBandDemodulate::m_ReAllocDynamicArrays() {
    m_FreeDynamicArrays();
    m_Z      = new int[m_Lsymbols];
    if (0L == m_Z) {
	fprintf(stderr,	"BaseBandDemodulate:: error: memory allocation failed!\n");
	return;
    }
    memset(m_Z, 0, m_Lsymbols*sizeof(int));

    m_baseBandSamples = new int[m_N];
    if (0L == m_baseBandSamples) {
	fprintf(stderr,	"BaseBandDemodulate:: error: memory allocation failed!\n");
	return;
    }
    memset(m_baseBandSamples, 0, m_N*sizeof(int));
} // BaseBandDemodulate::m_ReAllocDynamicArrays()

void BaseBandDemodulate::m_FreeDynamicArrays() {
    if (0L != m_Z) {
	if (1 >= m_Lsymbols) {
	    delete m_Z;
	}
	else {
	    delete [] m_Z;
	}
	m_Z = 0L;
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
} // BaseBandDemodulate::m_FreeDynamicArrays()

