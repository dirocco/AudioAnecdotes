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

#ifndef INCLUDED_STOCHASTICAMPLITUDESUSTAINSKEWESSENCE_H
#define INCLUDED_STOCHASTICAMPLITUDESUSTAINSKEWESSENCE_H

#include <cassert>

#include "Random.h"
#include "BasicAmplitudeSustainSkewEssence.h"

/*
    StochasticAmplitudeSustainSkewEssence models AmplitudeAttackSustainReleaseEssenceT
*/

class StochasticAmplitudeSustainSkewEssence {
public:
    StochasticAmplitudeSustainSkewEssence()
        : amplitudeGenerator_( 0., 1. )
        , skewGenerator_( -1., 1. )
        , sustainGenerator_( 0., 1. ) {}

    StochasticAmplitudeSustainSkewEssence& amplitude( double min, double max ) {
        amplitudeGenerator_.range( min, max );
        return *this;
    }

    StochasticAmplitudeSustainSkewEssence& sustain( double min, double max ) {
        assert( min >= 0. && max <= 1. );
        sustainGenerator_.range( min, max );
        return *this;
    }
    
    StochasticAmplitudeSustainSkewEssence& skew( double min, double max ) {
        assert( min >= .1 && max <= 1. );
        skewGenerator_.range( min, max );
        return *this;
    }

    void makeAmplitudeAttackSustainRelease( size_t duration,
            float& amplitude, size_t& attack, size_t& sustain, size_t& release ) const {

        amplitude = amplitudeGenerator_.generate();

        BasicAmplitudeSustainSkewEssence::durationSustainSkewToAttackSustainRelease(
            duration, sustainGenerator_.generate(), skewGenerator_.generate(),
            attack, sustain, release );
    }

private:
    mutable RangedDoubleRandomNumberGenerator amplitudeGenerator_;
    mutable RangedDoubleRandomNumberGenerator sustainGenerator_;
    mutable RangedDoubleRandomNumberGenerator skewGenerator_;
};

#endif /* INCLUDED_STOCHASTICAMPLITUDESUSTAINSKEWESSENCE_H */
