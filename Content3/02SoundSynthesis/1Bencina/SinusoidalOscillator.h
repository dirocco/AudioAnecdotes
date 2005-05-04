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

#ifndef INCLUDED_SINUSOIDALOSCILLATOR_H
#define INCLUDED_SINUSOIDALOSCILLATOR_H

#include <cmath>

class SinusoidalOscillator{
public:
    SinusoidalOscillator() : sin_( 0. ), cos_( 1. ) {}
    
    SinusoidalOscillator( double initialPhaseInRadians )
        : sin_( std::sin( initialPhaseInRadians ) )
        , cos_( std::cos( initialPhaseInRadians ) ) {}

    SinusoidalOscillator( double initialSineValue, double initialCosineValue )
        // assert( asin( initialSineValue ) == acos( initialCosineValue ) );
        : sin_( initialSineValue )
        , cos_( initialCosineValue ) {}
    
    SinusoidalOscillator& frequency( double frequency, double sampleRate ){
        double incrementAngle = 2. * M_PI * (frequency / sampleRate);
        a_ = std::sin( .5 * incrementAngle );
        a_ = 2. * a_ * a_;
        b_ = std::sin( incrementAngle );
        return *this;
    }

    SinusoidalOscillator& phase( double phaseInRadians ){
        sin_ = std::sin( phaseInRadians );
        cos_ = std::cos( phaseInRadians );
        return *this;
    }

    SinusoidalOscillator& phase( double sin, double cos ){
        sin_ = sin;
        cos_ = cos;
        return *this;
    }
    
    SinusoidalOscillator& operator++() {
        double temp = cos_ - (a_*cos_ + b_*sin_);
        sin_ = sin_ - (a_*sin_ - b_*cos_);
        cos_ = temp;
        return *this;
    }

    double operator*() const { return sin_; }

    double sin() const { return sin_; }
    double cos() const { return cos_; }

private:
    double cos_, sin_;
    double a_, b_;
};

#endif /* INCLUDED_SINUSOIDALOSCILLATOR_H */
