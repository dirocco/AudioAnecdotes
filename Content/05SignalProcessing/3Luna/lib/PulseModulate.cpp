////////////////////////////////////////////////////////////////////////////////
//
// PulseModulate.cpp
//
// Pulse code modulate(PCM) M-ary symbols into baseband digital signal
// pi(t).  Currently outputs pi(t) in NRZ-L form, i.e. nonreturn to
// zero - level, at voltage level V.
//
// Note: It would be very easy to add several other PCM methods here
// for experimentation with system performance.  See the
// FormatSignalNRZL() method at the end of this file.
//
// Assumes that the M from the M-ary symbols corresponds to k, where 0 < k <=8,
// and M = 2^^k
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "PulseModulate.h"

PulseModulate::~PulseModulate() {
    memset(m_signal, 0, 8*sizeof(int));
} // PulseModulate::~PulseModulate()

// set the current symbol to be formatted and the number of bits
// per symbol.  There are M = 2^^k symbols in the symbol set, aka
// alphabet. 0 < k <= 8 for simplicity.
void PulseModulate::SetSymbol(unsigned char symbol, unsigned k) {
    // validate k
    if (0 == k || k > 8) {
        return;
    }       
    m_k = k;
    m_symbol = symbol;
    FormatSignalNRZL();
} // PulseModulate::SetSymbol()

// return pi(n*T) where 0 <= n < k
int PulseModulate::GetPCMSignal(unsigned n) {
    // validate n
    if (n >= m_k) {
        return 0xffffffff;
    }
    else {
	return m_V*m_signal[n];
    }
} // PulseModulate::GetSignal()

// return the voltage level
int PulseModulate::GetVoltageLevel() {
    return m_V;
} // PulseModulate::GetVoltageLevel()

// set the voltage level
void PulseModulate::SetVoltageLevel(int V) {
    m_V = V;
} // PulseModulate::SetVoltageLevel()

// Built-in test routine.
void PulseModulate::Test() {
    printf("\n<-----     Begin Testing PulseModulate    ----->\nExpected Output:\n");
    unsigned int i;
    for (i=0; i < 8; i++) {
	printf("pi(%d*T) ", i);
    }
    printf("\n");
    for (i=0; i < 8; i++) {
	printf("%7d ", ((0xbe>>(7-i)) & 0x1) ? 1: -1);
    }
    printf("\nTest Output:\n");
    SetSymbol(0xbe, 8);
    for (i=0; i < 8; i++) {
	printf("%7d ", GetPCMSignal(i));
    }
    printf("\nTest Output at Voltage Level= %d\n", 0x6000);
    SetSymbol(0xbe, 8);
    SetVoltageLevel(0x6000);
    for (i=0; i < 8; i++) {
	printf("%7d ", GetPCMSignal(i));
    }
    printf("\n");
    printf("\n<-----      End Testing PulseModulate     ----->\n");
} // PulseModulate::Test()

// Implements NRZ-L PCM method.
// Note: It would be very easy to add several other PCM methods here
// for experimentation with system performance.
void PulseModulate::FormatSignalNRZL() {
    for (unsigned int i = 1; i <= m_k; i++) {
	m_signal[i-1] = ((m_symbol>>(m_k-i)) & 0x1) ? 1: -1;
    }
} // PulseModulate::FormatSignalNRZL()
