////////////////////////////////////////////////////////////////////////////////
//
// BaseBandDemodulate.h
//
// BaseBand demodulate incoming baseband digital signal (r(t)) samples
// by application of a matched filter; in this case a root raised
// cosine pulse.  r(t) is treated as a one bit signal(k=1) sampled N
// times, so if k > 1, multiply system symbolRate by k for this
// subsystem.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _BaseBandDemodulate_h_
#define _BaseBandDemodulate_h_

class BaseBandDemodulate {
 public:
    BaseBandDemodulate();
    ~BaseBandDemodulate();

    // sets r(t) where 0 <= n < N
    void SetBaseBandDigitalSignalSample(unsigned n, int value);

    // gets filter sum, Z(T)
    int GetFilterSum();

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

    //
    // Sliding window of the outgoing matched filter sums, Z[N-1] == Z(T)
    // These are suitable for passing to a signal detector.
    //
    int     *m_Z;

    // output baseband digital signal samples
    int     *m_baseBandSamples;

    // essentially a FIFO array en-queuing.  Drops shifted off value.
    void m_ShiftMatchedFilterOutput(int value);

    //
    // BaseBand demodulate incoming baseband digital signal (r(t)) samples
    // by application of a matched filter.
    //
    void m_Demodulate();

    //
    // nyquist root raised cosine pulse centered at t=0
    //
    double m_z(double t);

    void m_ReAllocDynamicArrays();
    void m_FreeDynamicArrays();
};

#endif _BaseBandDemodulate_h_
