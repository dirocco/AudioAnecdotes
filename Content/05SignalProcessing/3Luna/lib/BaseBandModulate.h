////////////////////////////////////////////////////////////////////////////////
//
// BaseBandModulate.h
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

#ifndef _BaseBandModulate_h_
#define _BaseBandModulate_h_

class BaseBandModulate {
 public:
    BaseBandModulate();
    ~BaseBandModulate();

    // sets pi(n*T) where 0 <= n < k
    void SetPCMSignal(int p);

    // gets gi(t) where 0 <= n < N
    int GetBaseBandDigitalSignalSample(unsigned n);

    //
    // Set/Get Nyquist Roll-off for the root raised cosine filter
    // 0.0 <= r <= 1.0
    //
    // Note: The following function may be helpful for determining a
    // value for Lsymbols:
    //   Lsymbols = -44*R + 33 where 0.2 < R < 0.75
    // 
    void  SetNyquistRollOff(double r);
    double GetNyquistRollOff();

    // symbolRate > 0
    void     SetSymbolRate(unsigned symbolRate);
    unsigned GetSymbolRate();

    // Lsymbols > 0.  See note on SetNyquistRollOff().
    void     SetPulseSymbolSpan(unsigned Lsymbols);
    unsigned GetPulseSymbolSpan();

    // N > 0
    void     SetSamplesPerSymbol(unsigned N);
    unsigned GetSamplesPerSymbol();

    //
    // Use GnuplotDriver.AppendStreamData() calls for plotting filter
    // function.  The caller is responsible for calling
    // GnuplotDriver.PlotStreams() manually.
    //
    void PlotFilter();

    // Built-in test routine.
    void Test();

 private:
    // nyquist roll off factor (R)
    double    m_nyquistRollOff;

    // symbol tranmission rate
    unsigned m_symbolRate;

    // symbol tranmission period computed by 1/m_symbolRate
    double    m_Tsymbol;

    // scale factor to normalize output of m_z(0.0) to 1.0
    double    m_scaleFactor;

    // passband pulse samples/symbol
    unsigned m_N;

    unsigned m_Lsymbols;

    // sliding window of the incoming PCM digital waveform
    // values(pi(nT) or symbols)
    int     *m_PCMSymbols;

    // output baseband digital signal samples
    int     *m_baseBandSamples;

    // essentially a FIFO array en-queuing.  Drops shifted off value.
    void m_ShiftSymbols(int value);

    //
    // compute baseband digital signal output samples(gi(t)) from (m_Lsymbols)
    // span of input PCM symbols modulated with z(t)
    //
    void m_Modulate();

    //
    // nyquist root raised cosine pulse centered at t=0
    //
    double m_z(double t);

    void m_ReAllocDynamicArrays();
    void m_FreeDynamicArrays();
};

#endif _BaseBandModulate_h_
