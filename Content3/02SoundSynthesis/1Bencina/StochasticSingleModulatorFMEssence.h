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

#ifndef INCLUDED_STOCHASTICSINGLEMODULATORFMESSENCE_H
#define INCLUDED_STOCHASTICSINGLEMODULATORFMESSENCE_H

#include <cassert>

#include "Random.h"

/*
    StochasticSingleModulatorFMEssence models SingleModulatorFMEssenceT

    concept SingleModulatorFMEssenceT{
        double sampleRate() const;
        float modulatorFrequency() const;
        float carrierFrequency() const;
        float modulationIndex() const;
    }
*/

class StochasticSingleModulatorFMEssence {
public:
    StochasticSingleModulatorFMEssence()
        : modulatorFrequencyGenerator_( 0., 1. )
        , carrierFrequencyGenerator_( -1., 1. )
        , modulationIndexGenerator_( 0., 1. )
        , sr_( 44100. ){}

    StochasticSingleModulatorFMEssence& sampleRate( double sr ) {
        sr_ = sr;
        return *this;
    }

    StochasticSingleModulatorFMEssence& modulatorFrequency( double min, double max ) {
        modulatorFrequencyGenerator_.range( min, max );
        return *this;
    }

    StochasticSingleModulatorFMEssence& carrierFrequency( double min, double max ) {
        carrierFrequencyGenerator_.range( min, max );
        return *this;
    }

    StochasticSingleModulatorFMEssence& modulationIndex( double min, double max ) {
        modulationIndexGenerator_.range( min, max );
        return *this;
    }


    double sampleRate() const { return sr_; }
    float modulatorFrequency() const { return modulatorFrequencyGenerator_.generate(); }
    float carrierFrequency() const { return carrierFrequencyGenerator_.generate(); }
    float modulationIndex() const { return modulationIndexGenerator_.generate(); }

private:
    mutable RangedDoubleRandomNumberGenerator modulatorFrequencyGenerator_;
    mutable RangedDoubleRandomNumberGenerator carrierFrequencyGenerator_;
    mutable RangedDoubleRandomNumberGenerator modulationIndexGenerator_;
    double sr_;
};

#endif /* INCLUDED_STOCHASTICSINGLEMODULATORFMESSENCE_H */
