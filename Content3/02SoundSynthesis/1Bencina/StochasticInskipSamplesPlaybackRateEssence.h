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

#ifndef INCLUDED_STOCHASTICINSKIPSAMPLESPLAYBACKRATEESSENCE_H
#define INCLUDED_STOCHASTICINSKIPSAMPLESPLAYBACKRATEESSENCE_H

#include "Random.h"

/*
    StochasticInskipSamplesPlaybackRateEssence models:

    concept InskipSamplesPlaybackRateEssenceT{
        Sample& sample() const;
        double inskipSamples() const;
        double playbackRate() const;
    }
*/

class Sample;

class StochasticInskipSamplesPlaybackRateEssence{
public:
    StochasticInskipSamplesPlaybackRateEssence()
        : sample_( NULL )
        , inskipSamplesGenerator_( 0., 1000.)
        , playbackRateGenerator_( 1., 1. ) {}

    StochasticInskipSamplesPlaybackRateEssence& sample( Sample *sample )
        { sample_ = sample; return *this; }

    StochasticInskipSamplesPlaybackRateEssence& inskipSamples( double min, double max )
        { inskipSamplesGenerator_.range( min, max ); return *this; }

    StochasticInskipSamplesPlaybackRateEssence& playbackRate( double min, double max )
        { playbackRateGenerator_.range( min, max ); return *this; }


    Sample& sample() const { return *sample_; }
    double inskipSamples() const { return inskipSamplesGenerator_.generate(); }
    double playbackRate() const { return playbackRateGenerator_.generate(); }

private:
    Sample *sample_;
    mutable RangedDoubleRandomNumberGenerator inskipSamplesGenerator_;
    mutable RangedDoubleRandomNumberGenerator playbackRateGenerator_;
};

#endif /* INCLUDED_STOCHASTICINSKIPSAMPLESPLAYBACKRATEESSENCE_H */
