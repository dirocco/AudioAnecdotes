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

#ifndef INCLUDED_STOCHASTICDELAYSAMPLESPLAYBACKRATEESSENCE_H
#define INCLUDED_STOCHASTICDELAYSAMPLESPLAYBACKRATEESSENCE_H

#include "Random.h"

/*
    StochasticDelaySamplesPlaybackRateEssence models:
    
    concept DelaySamplesPlaybackRateEssenceT{
        DelayLine& delayLine() const;
        double delaySamples() const;
        double playbackRate() const;
    }
*/

class DelayLine;

class StochasticDelaySamplesPlaybackRateEssence{
public:
    StochasticDelaySamplesPlaybackRateEssence()
        : delayLine_( NULL )
        , delaySamplesGenerator_( 100., 1000.)
        , playbackRateGenerator_( 1., 1. ) {}

    StochasticDelaySamplesPlaybackRateEssence& delayLine( DelayLine *delayLine )
        { delayLine_ = delayLine; return *this; }
        
    StochasticDelaySamplesPlaybackRateEssence& delaySamples( double min, double max )
        { delaySamplesGenerator_.range( min, max ); return *this; }

    StochasticDelaySamplesPlaybackRateEssence& playbackRate( double min, double max )
        { playbackRateGenerator_.range( min, max ); return *this; }


    DelayLine& delayLine() const { return *delayLine_; }
    double delaySamples() const { return delaySamplesGenerator_.generate(); }
    double playbackRate() const { return playbackRateGenerator_.generate(); }

private:
    DelayLine *delayLine_;
    mutable RangedDoubleRandomNumberGenerator delaySamplesGenerator_;
    mutable RangedDoubleRandomNumberGenerator playbackRateGenerator_;
};

#endif /* INCLUDED_STOCHASTICDELAYSAMPLESPLAYBACKRATEESSENCE_H */
