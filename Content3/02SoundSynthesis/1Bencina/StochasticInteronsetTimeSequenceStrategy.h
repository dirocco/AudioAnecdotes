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

#ifndef INCLUDED_STOCHASTICINTERONSETTIMESEQUENCESTRATEGY_H
#define INCLUDED_STOCHASTICINTERONSETTIMESEQUENCESTRATEGY_H

#include <cmath>

#include "Random.h"

/*
    StochasticInteronsetTimeSequenceStrategy models SequenceStrategyT (see Scheduler.h)

    StochasticInteronsetTimeSequenceStrategy generates interonset times and
    grain durations using pseudo random number generators.
*/


class StochasticInteronsetTimeSequenceStrategy{
public:
    StochasticInteronsetTimeSequenceStrategy()
        : interonsetGenerator_( 441, 441 )       // initialise with sane initial values
        , durationGenerator_( 441, 441 ) {}

    StochasticInteronsetTimeSequenceStrategy& interonsetTime( double min, double max ) {
        interonsetGenerator_.range( min, max );
        return *this;
    }

    StochasticInteronsetTimeSequenceStrategy& duration( unsigned int min, unsigned int max ) {
        durationGenerator_.range( min, max );
        return *this;
    }

    double nextInteronset() { return interonsetGenerator_.generate(); }
    size_t nextDuration() { return durationGenerator_.generate(); }
    
private:
    RangedDoubleRandomNumberGenerator interonsetGenerator_;
    RangedLongRandomNumberGenerator durationGenerator_;
};

#endif /* INCLUDED_STOCHASTICINTERONSETTIMESEQUENCESTRATEGY_H */
