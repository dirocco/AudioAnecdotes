////////////////////////////////////////////////////////////////////////////////
//
// AWGNChannel.cpp
//
// Simulates noisy channel by applying additive white gaussian
// noise(AWGN) to transmitted signal.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "AWGNChannel.h"

// return the voltage level
int AWGNChannel::GetVoltageLevel() {
    return m_V;
} // AWGNChannel::GetVoltageLevel()

// set the voltage level
void AWGNChannel::SetVoltageLevel(int V) {
    m_V = V;
} // AWGNChannel::SetVoltageLevel()

double AWGNChannel::GetNoiseLevel() {
    return m_noiseLevel;
} // AWGNChannel::GetNoiseLevel()

void AWGNChannel::SetNoiseLevel(double noiseLevel) {
    if (noiseLevel <= 1.0f) {
	m_noiseLevel = noiseLevel;
    }
    else {
	fprintf(stderr,
		"SetNoiseLevel(%f): noise level passed should be <= 1.0\n");
    }
} // AWGNChannel::SetNoiseLevel()

int AWGNChannel::AddNoiseToSignal(int signal) {
    return (signal + int(m_V*m_AWGN(m_noiseLevel)));
} // AWGNChannel::AddNoiseToSignal()

// level=pow(10,noise_power in dB/10.0)
double AWGNChannel::m_AWGN(double level) {
    double a,b,r;

    a = rand()/double(RAND_MAX);
    b = 6.28*(rand()/double(RAND_MAX));
    if (a != 1.0) { 
	r = sqrt(2.0*level*-log(1.0-a))*cos(b);
	fprintf(stdout, "a = %10.10lf, r = %10.10lf\n", a, r);
    }
    else {
	r = sqrt(2.0*level*-log(1e-12))*cos(b);
	fprintf(stdout, "a = %10.10lf, r = %10.10lf\n", a, r);

    }
    return(r);
} // AWGNChannel::m_AWGN()
