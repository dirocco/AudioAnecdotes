// -*- C++ -*-	
////////////////////////////////////////////////////////////////////////////////
//
// PulseModulate.h
//
// Pulse code modulate(PCM) M-ary symbols into baseband digital signal pi(t).
// Outputs pi(t) in NRZ-L form, i.e. nonreturn to zero - level, at
// voltage level V.
//
// Note: It would be very easy to add several other PCM methods here
// for experimentation with system performance.  See the
// FormatSignalNRZL() method at the end of this file.
//
// Assumes that the M from the M-ary symbols corresponds to k, where 0 < k <=8,
// and M = 2^^k
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _PulseModulate_h_
#define _PulseModulate_h_

class PulseModulate {
public:
    PulseModulate() :
	m_symbol(0),
	m_k(0),
	m_V(1) {  memset(m_signal, 0, 8*sizeof(int));  }

    ~PulseModulate();

    // set the current symbol to be formatted and the number of bits
    // per symbol.  There are M = 2^^k symbols in the symbol set, aka
    // alphabet. 0 < k <= 8 for simplicity.
    void SetSymbol(unsigned char symbol, unsigned k);

    // return pi(n*T) where 0 <= n < k
    int GetPCMSignal(unsigned n);

    // return the voltage level
    int GetVoltageLevel();

    // set the voltage level
    void SetVoltageLevel(int V);

    // Built-in test routine.
    void Test();

private:
    int           m_signal[8];
    int           m_V;
    unsigned char m_symbol;
    unsigned      m_streamSymbolCount;
    unsigned      m_k;

    // Implements NRZ-L PCM method.
    // Note: It would be very easy to add several other PCM methods here
    // for experimentation with system performance.
    void FormatSignalNRZL();
};


#endif _PulseModulate_h_
