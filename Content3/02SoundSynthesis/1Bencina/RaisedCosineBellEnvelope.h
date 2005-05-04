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

#ifndef INCLUDED_RAISEDCOSINEBELLENVELOPE_H
#define INCLUDED_RAISEDCOSINEBELLENVELOPE_H

#include <ctype.h>
#include <cassert>
#include <cmath>

/*
    RaisedCosineBellEnvelope models GrainEnvelopeT (see Grain.h)

    requires:

    concept AmplitudeAttackSustainReleaseEssenceT{
        void getAmplitudeAttackSustainRelease( size_t duration, float&, size_t&, size_t&, size_t& );
    };
*/

template< class AmplitudeAttackSustainReleaseEssenceT >
class RaisedCosineBellEnvelope{
public:
    typedef AmplitudeAttackSustainReleaseEssenceT essence_type;
     
    RaisedCosineBellEnvelope( size_t duration, const essence_type& essence ) {

        float amplitude;
        essence.makeAmplitudeAttackSustainRelease( duration, amplitude, counter0_, counter1_, counter2_ );

        assert( counter0_ > 0 && counter0_ < duration );
        assert( counter1_ > 0 && counter1_ < duration );
        assert( counter2_ > 0 && counter2_ < duration );

		ampD2_ = amplitude *.5f;
		
		// attack coefficients:

		double w = M_PI / counter0_;
		b1_ = 2.0 * std::cos( w );
		double ip = M_PI * 1.5;
		y1_ = sin( ip );
		y2_ = sin( ip - w );
			
		// sustain phase and increment:

        initialPhase1_ = M_PI * .5;
		phaseIncrement1_ = 0.f;

		// release phase and increment:

        initialPhase2_ =  M_PI * .5;
		phaseIncrement2_ = M_PI / counter2_;
    }

    float operator*() const {
        return (y1_ + 1.) * ampD2_;
    }
    
    RaisedCosineBellEnvelope& advance(){
        double y0 = b1_ * y1_ - y2_;
		y2_ = y1_;
		y1_ = y0;
        return *this;
    }

    size_t nextBoundary( size_t maximum ) const {
        return std::min( maximum, counter0_ );
    }
    
    void checkBoundary( size_t samplesPassed ) {
        assert( samplesPassed <= counter0_ );
        counter0_ -= samplesPassed;
        if( counter0_ == 0){
			
			counter0_ = counter1_;
			counter1_ = counter2_;
			counter2_ = 0;
			
			b1_ = 2.0 * std::cos( phaseIncrement1_ );
			y1_ = std::sin( initialPhase1_ );
			y2_ = std::sin( initialPhase1_ - phaseIncrement1_ );

            phaseIncrement1_ = phaseIncrement2_;
			phaseIncrement2_ = 0.f;
            
			initialPhase1_ = initialPhase2_;
            initialPhase2_ = M_PI * 1.5;
		}
    }
    
    bool atEnd() const { return counter0_ == 0; }

private:
    double y1_, y2_, b1_;   // these need to be double precision for long grains
    float ampD2_;
    
    size_t counter0_, counter1_, counter2_;
    float initialPhase1_, phaseIncrement1_;
    float initialPhase2_, phaseIncrement2_;
};

#endif /* INCLUDED_RAISEDCOSINEBELLENVELOPE_H */
