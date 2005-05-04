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

#ifndef INCLUDED_SINUSOIDALSOURCE_H
#define INCLUDED_SINUSOIDALSOURCE_H

#include <ctype.h>

#include "SinusoidalOscillator.h"

/*
    SinusoidalSource models GrainSourceT (see Grain.h)

    requires

    concept SampleRateFrequencyPhaseEssenceT{
        double sampleRate() const;
        float frequency() const;
        float phase() const;
    }
*/

template< class SampleRateFrequencyPhaseEssenceT >
class SinusoidalSource{
public:
    typedef SampleRateFrequencyPhaseEssenceT essence_type;
     
    SinusoidalSource( size_t, size_t, const essence_type& essence )
        : oscillator_( essence.phase() ) {

        oscillator_.frequency( essence.frequency(), essence.sampleRate() );
    }

    float operator()( float envelopeValue){
        return envelopeValue * *++oscillator_;
    }

    size_t nextBoundary( size_t maximum ) const { return maximum; }
    
    void checkBoundary( size_t ) {}

private:
    SinusoidalOscillator oscillator_;
};

#endif /* INCLUDED_SINUSOIDALSOURCE_H */
