// -*- C++ -*-	
////////////////////////////////////////////////////////////////////////////////
//
// AWGNChannel.h
//
// Simulates noisy channel by applying additive white gaussian
// noise(AWGN) to transmitted signal.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _AWGNChannel_h_
#define _AWGNChannel_h_

class AWGNChannel {
 public:
    AWGNChannel(): m_V(0x8fff), m_noiseLevel(0.1f) {}
	
    // set the voltage level
    void SetVoltageLevel(int V);

    // return the voltage level
    int GetVoltageLevel();

    // noise level corresponds to:
    // SNR = Eb/N0 = Eb/noiseLevel,
    // when bit energy Eb = 1, then
    // SNR = 1/noiseLevel
    // Frequently SNR is given in dB > 0, so to compute non-dB noiseLevel, just
    // perform 1/(10^^SNR) to get noiseLevel.
	
    // set the noise level
    void SetNoiseLevel(double noiseLevel);

    // returns the noise level
    double GetNoiseLevel();

    int AddNoiseToSignal(int signal);

 private:
    int           m_V;
    double        m_noiseLevel;
	
    // level=pow(10,noise_power in dB/10.0)
    double m_AWGN(double level);
};

#endif _AWGNChannel_h_
