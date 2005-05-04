/*
    Ross Bencina's Granular Synthesis Toolkit (RB-GST)
    Copyright (C) 2001 Ross Bencina.
    email: rossb@audiomulch.com
    web: http://www.audiomulch.com/~rossb/

    This file is part of RB-GST.

    RB-GST is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    RB-GST is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RB-GST; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef INCLUDED_SINUSOIDALFMOSCILLATOR_H
#define INCLUDED_SINUSOIDALFMOSCILLATOR_H

#include <limits>
#include <cmath>

/*
    A sinusoidal fm oscillator using an integer phasor and
    a taylor polynomial waveform.
*/

class SinusoidalFMOscillator{
public:
    SinusoidalFMOscillator()
        : index_(0)
        , increment_(0)
        , longToPlusMinusPi_( M_PI / (double)std::numeric_limits<signed long>::max() )
        , modulatorToLong_( (double)std::numeric_limits<signed long>::max() / M_PI ) {}

    SinusoidalFMOscillator& frequency( double frequency, double sampleRate ){
        increment_ = (frequency / sampleRate) * ((double)std::numeric_limits<signed long>::max() - (double)std::numeric_limits<signed long>::min());
        return *this;
    }

    // expects modulator to have the amplitude range (+/- I) as in
    // the standard FM equation: e = sin( at + I sin(bt) )
    float operator()( float modulator ){
        index_ += increment_ + ( modulator * modulatorToLong_ );

        double x = index_ * longToPlusMinusPi_;
        double x2 = x * x;
        double x3 = x * x * x;
        double result = x;
        double x5 = x3 * x2;
        result -= x3 * 0.166666666667;
        double x7 = x5 * x2;
        result += x5 * 0.008333333333;
        double x9 = x7 * x2;
        result -= x7 * 0.000198412698;
        result += x9 * 0.000002755732;
        return result;
    }

private:
    signed long index_;
    signed long increment_;
    double longToPlusMinusPi_;
    double modulatorToLong_;
};

#endif /* INCLUDED_SINUSOIDALFMOSCILLATOR_H */
