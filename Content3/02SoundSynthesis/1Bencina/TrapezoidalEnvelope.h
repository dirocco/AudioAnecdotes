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

#ifndef INCLUDED_TRAPEZOIDALENVELOPE_H
#define INCLUDED_TRAPEZOIDALENVELOPE_H

#include <ctype.h>
#include <cassert>

/*
    TrapezoidalEnvelope models GrainEnvelopeT (see Grain.h)

    requires:

    concept AmplitudeAttackSustainReleaseEssenceT{
        void makeAmplitudeAttackSustainRelease( size_t duration, float&, size_t&, size_t&, size_t& );
    };
*/

template< class AmplitudeAttackSustainReleaseEssenceT >
class TrapezoidalEnvelope{
public:
    typedef AmplitudeAttackSustainReleaseEssenceT essence_type;
     
    TrapezoidalEnvelope( size_t duration, const essence_type& essence )
            : value_( 0.f ) {

        float grainAmplitude;
        essence.makeAmplitudeAttackSustainRelease(
                duration, grainAmplitude, counter0_, counter1_, counter2_ );

        assert( counter0_ > 0 );
        assert( counter1_ > 0 );
        assert( counter2_ > 0 );

		inc0_ = grainAmplitude / counter0_;
        inc1_ = 0.f;
		inc2_ = - (grainAmplitude / counter2_);
    }

    float operator*() const {
        return value_;
    }
    
    TrapezoidalEnvelope& advance(){
        value_ += inc0_;
        return *this;
    }
    
    size_t nextBoundary( size_t maximum ) const {
        return std::min( maximum, counter0_ );
    }
    
    void checkBoundary( size_t samplesPassed ) {
        assert( samplesPassed <= counter0_ );
        counter0_ -= samplesPassed;
        if( counter0_ == 0 ){
            counter0_ = counter1_;
            inc0_ = inc1_;
            counter1_ = counter2_;
            inc1_ = inc2_;
            counter2_ = 0;
            inc2_ = 0.f;
        }
    }
    
    bool atEnd() const { return counter0_ == 0; }

private:
    float value_;
    size_t counter0_;
    float inc0_;
    size_t counter1_;
    float inc1_;
    size_t counter2_;
    float inc2_;
};

#endif /* INCLUDED_TRAPEZOIDALENVELOPE_H */
