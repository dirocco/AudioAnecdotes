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

#ifndef INCLUDED_STOCHASTICFMGRANULATOR_H
#define INCLUDED_STOCHASTICFMGRANULATOR_H

#include "Sample.h"

#include "SingleModulatorFMSource.h"
#include "StochasticSingleModulatorFMEssence.h"

#include "ParabolicEnvelope.h"
#include "StochasticAmplitudeEssence.h"

#include "StochasticDensitySequenceStrategy.h"

#include "Grain.h"
#include "Scheduler.h"


class StochasticFMGranulator{
public:
    typedef Grain<  SingleModulatorFMSource<StochasticSingleModulatorFMEssence>, StochasticSingleModulatorFMEssence,
                    ParabolicEnvelope<StochasticAmplitudeEssence>, StochasticAmplitudeEssence > grain_type;

    typedef Scheduler< StochasticDensitySequenceStrategy, grain_type > scheduler_type;


    StochasticFMGranulator( int maxGrainPolyphony,
                                    double sampleRate )
        : scheduler_( maxGrainPolyphony )
        , sr_( sampleRate ) {

        scheduler_.sourceEssence.sampleRate( sr_ );
    }

    StochasticFMGranulator& sampleRate( double x ){
        sr_ = x;
        scheduler_.sourceEssence.sampleRate( x );
        return *this;
    }

    StochasticFMGranulator& density( double grainsPerSecond ){ // seconds
        scheduler_.sequenceStrategy.density( grainsPerSecond, sr_ );
        return *this;
    }

    StochasticFMGranulator& grainDuration( double min, double max ){ // seconds
        scheduler_.sequenceStrategy.duration( min*sr_, max*sr_ );
        return *this;
    }

    StochasticFMGranulator& amplitude( double min, double max ){
        scheduler_.envelopeEssence.amplitude( min, max );
        return *this;
    }

    StochasticFMGranulator& modulatorFrequency( double min, double max ){ // seconds
        scheduler_.sourceEssence.modulatorFrequency( min, max );
        return *this;
    }

    StochasticFMGranulator& carrierFrequency( double min, double max ){
        scheduler_.sourceEssence.carrierFrequency( min, max );
        return *this;
    }

    StochasticFMGranulator& modulationIndex( double min, double max ){
        scheduler_.sourceEssence.modulationIndex( min, max );
        return *this;
    }

    void synthesize( float *output, size_t length ){
        scheduler_.synthesize( output, length );
    }

private:
    scheduler_type scheduler_;
    double sr_;
};                       

#endif /* INCLUDED_STOCHASTICFMGRANULATOR_H */
