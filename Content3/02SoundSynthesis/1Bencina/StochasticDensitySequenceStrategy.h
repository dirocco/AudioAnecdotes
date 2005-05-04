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

#ifndef INCLUDED_STOCHASTICDENSITYSEQUENCESTRATEGY_H
#define INCLUDED_STOCHASTICDENSITYSEQUENCESTRATEGY_H

#include <cmath>
#include <ctype.h>

#include "Random.h"

/*
    StochasticDensitySequenceStrategy models SequenceStrategyT (see Scheduler.h)

    StochasticDensitySequenceStrategy generates interonset times which on
    average result in a constant grain density per unit time, which can
    be specified using the density() method.
*/


class StochasticDensitySequenceStrategy{
public:
StochasticDensitySequenceStrategy()
        : iDxSR( 441. )
        , durationGenerator_( 441, 441 ) {} // initialise with sane initial values

    StochasticDensitySequenceStrategy& density( double grainsPerSecond, double sampleRate ) {
        iDxSR = sampleRate / grainsPerSecond;
        return *this;
    }

    StochasticDensitySequenceStrategy& duration( unsigned int min, unsigned int max ) {
        durationGenerator_.range( min, max );
        return *this;
    }

    double nextInteronset() { return -log( interonsetGenerator_.generate() ) * iDxSR; }
    size_t nextDuration() { return durationGenerator_.generate(); }
    
private:
    UnitDoubleRandomNumberGenerator interonsetGenerator_;
    double iDxSR;   // inverse density * sampleRate
    RangedLongRandomNumberGenerator durationGenerator_;
};

#endif /* INCLUDED_STOCHASTICDENSITYSEQUENCESTRATEGY_H */
