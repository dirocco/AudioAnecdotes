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

#ifndef INCLUDED_FOFGENERATOR_H
#define INCLUDED_FOFGENERATOR_H

#include "SinusoidalSource.h"
#include "BasicSampleRateFrequencyPhaseEssence.h"

#include "FofEnvelope.h"

#include "PeriodicSequenceStrategy.h"

#include "Grain.h"
#include "Scheduler.h"

/*  This file implements FOF synthesis as described in:
    Rodet, X., Potard, Y., Barrière, J.B. 1984. "The CHANT project :
    from the synthesis of the singing voice to synthesis in general."
    In C. Roads (ed.), The Music Machine, Cambridge, MA: MIT Press.
*/
                         
class FofGenerator{
public:
    typedef Grain<  SinusoidalSource<BasicSampleRateFrequencyPhaseEssence>, BasicSampleRateFrequencyPhaseEssence,
                    FofEnvelope, FofEnvelopeEssence > grain_type;

    typedef Scheduler< PeriodicSequenceStrategy, grain_type > scheduler_type;


    FofGenerator( int maxGrainPolyphony )
        : scheduler_( maxGrainPolyphony ) {}

    FofGenerator& sampleRate( double x ){
        sr_ = x;
        scheduler_.sourceEssence.sampleRate(x);
        scheduler_.envelopeEssence.sampleRate(x);
        return *this;
    }


    FofGenerator& fundamentalFrequency( double x ){
        scheduler_.sequenceStrategy.frequencyOverlap( x, scheduler_.maxGrains(), sr_ );
        return *this;
    }

    FofGenerator& formantCenterFrequency( double x ){
        scheduler_.sourceEssence.frequency(x);
        return *this;
    }

    FofGenerator& formantBandwidth( double x ){
        scheduler_.envelopeEssence.bandwidth(x);
        return *this;
    }

    FofGenerator& formantSkirtWidth( double x ){
        scheduler_.envelopeEssence.skirtWidth(x);
        return *this;
    }

    FofGenerator& formantAmplitude( double x ){
        scheduler_.envelopeEssence.amplitude(x);
        return *this;
    }

    FofGenerator& formantPhase( double x ){ // radians
        scheduler_.sourceEssence.phase(x);
        return *this;
    }

    void synthesize( float *output, size_t length ){
        scheduler_.synthesize( output, length );
    }

private:
    scheduler_type scheduler_;
    double sr_;
};


#endif /* INCLUDED_FOFGENERATOR_H */
