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

#ifndef INCLUDED_PARABOLICENVELOPE_H
#define INCLUDED_PARABOLICENVELOPE_H

#include <ctype>
#include <cassert>

/*
    ParabolicEnvelope models GrainEnvelopeT (see Grain.h)

    requires:

    concept AmplitudeEssenceT{
        float amplitude() const;
    };
*/

template< class AmplitudeEssenceT >
class ParabolicEnvelope{
public:
    typedef AmplitudeEssenceT essence_type;
     
    ParabolicEnvelope( size_t duration, const essence_type& essence )
        : countdown_( duration )
        , amplitude_( 0.f ) {
		
		float rdur = 1.f / countdown_;
		float rdur2 = rdur * rdur;
		
		float grainAmplitude = essence.amplitude();
		
		slope_ = 4.f * grainAmplitude * ( rdur - rdur2 );
		curve_ = -8.f * grainAmplitude * rdur2;
    }

    float operator*() const {
        return amplitude_;
    }
    
    ParabolicEnvelope& advance() {
        amplitude_ += slope_;
		slope_ += curve_;
		return *this;
    }
    
    size_t nextBoundary( size_t maximum ) const {
        return std::min( maximum, countdown_ );
    }
    
    void checkBoundary( size_t samplesPassed ) {
        assert( samplesPassed <= countdown_ );
        countdown_ -= samplesPassed;
        if( countdown_ == 0 ){
            slope_ = 0.f;
            curve_ = 0.f;
        }
    }
    
    bool atEnd() const { return countdown_ == 0; }

private:
    size_t countdown_;
	float amplitude_;
    float slope_;
    float curve_;
};

#endif /* INCLUDED_PARABOLICENVELOPE_H */
