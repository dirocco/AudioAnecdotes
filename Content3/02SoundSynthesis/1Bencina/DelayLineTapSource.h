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

#ifndef INCLUDED_DELAYLINETAPSOURCE_H
#define INCLUDED_DELAYLINETAPSOURCE_H

#include <ctype.h>

#include "DelayLine.h"

/*
    DelayLineTapSource models GrainSourceT (see Grain.h)

    requires

    concept DelaySamplesPlaybackRateEssenceT{
        DelayLine& delayLine() const;
        double delaySamples() const;
        double playbackRate() const;
    }
*/

template< class DelaySamplesPlaybackRateEssenceT >
class DelayLineTapSource{
public:
    typedef DelaySamplesPlaybackRateEssenceT essence_type;
     
    DelayLineTapSource( size_t vectorInskip, size_t duration, const essence_type& essence )
        : delayTap_( essence.delayLine(),
            calcInitialDelay( duration, essence.delaySamples()-vectorInskip, essence.playbackRate() ),
            essence.playbackRate() ) {}

    float operator()( float envelopeValue ){
        float result = envelopeValue * *delayTap_;
        ++delayTap_;
        return result;
    }

    size_t nextBoundary( size_t maximum ) const { return maximum; }
    
    void checkBoundary( size_t ) {}

private:
    DelayLine::FixedRateTap delayTap_;

    static double calcInitialDelay( size_t duration, double requestedDelaySamples, double playbackRate ){
        double delaySamples = requestedDelaySamples;
        if( (delaySamples + duration) / playbackRate < duration )
            delaySamples = (duration * playbackRate) - duration;

        return delaySamples;
    }
};

#endif /* INCLUDED_DELAYLINETAPSOURCE_H */
