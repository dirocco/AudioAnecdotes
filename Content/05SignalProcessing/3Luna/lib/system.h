// -*- C++ -*-	
////////////////////////////////////////////////////////////////////////////////
//
// system.h
//
// BaseBand modem system settings
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _system_h_
#define _system_h_

#include <math.h>
#include <limits.h>

typedef enum {
	SHORT_DATA = 0,
	INT_DATA,
	FLOAT_DATA
} DATA_TYPE;

//
// system values.  Passed into subsystems
//
double pi          = 2.0*atan2(1.0,0.0);

// M = 2^^k
const unsigned k    = 2;

// N samples per symbol
unsigned N          = 4;

// Lsymbols = -44*R + 33
// where R is nyquist roll-off with range 0.2 < R < 0.75
// from http://www.ee.byu.edu/ee/class/ee485/lectures/raised-cosine.pdf
// pulse span is -Lsymbol/2*Ts <= t <=  Lsymbol/2*Ts)

// corresponds with R = 0.4
unsigned Lsymbols   =  16;

// nyquist roll-off
double   R          = 0.4;

// PCM voltage level
//
// This voltage level is chosen as USHRT_MAX since we will be
// multiplying it by float values in the Baseband modulation stage.
// This will give us good precision, while leaving lots of room for
// our channel noise.
// 
unsigned V          = 0x6000; //USHRT_MAX; 

// noise level(sometimes referred to as NSR) corresponds to:
// SNR = Eb/N0 = Eb/noiseLevel,
// when bit energy Eb = 1, then
// SNR = 1/noiseLevel, noiseLevel = 1/SNR
// Frequently SNR is given in dB > 0, so to compute non-dB noiseLevel, just
// perform 1/(10^^SNR) to get noiseLevel.
double   noiseLevel = 0.0999;


unsigned bandwidth  = 44100;
unsigned symbolRate = unsigned(k*44100.0/(2.0*(1.0+R)));
double   Tsymbol    = 1.0/double(symbolRate);

#endif _system_h_
