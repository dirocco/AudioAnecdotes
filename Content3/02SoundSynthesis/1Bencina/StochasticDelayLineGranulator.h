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

#ifndef INCLUDED_STOCHASTICDELAYLINEGRANULATOR_H
#define INCLUDED_STOCHASTICDELAYLINEGRANULATOR_H

#include "DelayLine.h"
#include "DCBlocker.h"
#include "Random.h"

#include "DelayLineTapSource.h"
#include "StochasticDelaySamplesPlaybackRateEssence.h"

#include "TrapezoidalEnvelope.h"
#include "StochasticAmplitudeSustainSkewEssence.h"

#include "StochasticInteronsetTimeSequenceStrategy.h"

#include "Grain.h"
#include "Scheduler.h"

                         
class StochasticDelayLineGranulator{
public:
    typedef Grain<  DelayLineTapSource<StochasticDelaySamplesPlaybackRateEssence>, StochasticDelaySamplesPlaybackRateEssence,
                    TrapezoidalEnvelope<StochasticAmplitudeSustainSkewEssence>, StochasticAmplitudeSustainSkewEssence > grain_type;

    typedef Scheduler< StochasticInteronsetTimeSequenceStrategy, grain_type > scheduler_type;


    StochasticDelayLineGranulator( int maxGrainPolyphony,
                                    double maxDelaySeconds,
                                    double sampleRate )
        : scheduler_( maxGrainPolyphony )
        , delayLine_( maxDelaySeconds * sampleRate ) // could be longer to account for playback rates > 1.
        , smallNoise_( -.000000001, .000000001 )
        , sr_( sampleRate )
        , feedback_( 0.f ) {

        scheduler_.sourceEssence.delayLine( &delayLine_ );
    }

    StochasticDelayLineGranulator& sampleRate( double x ){
        sr_ = x;
        return *this;
    }

    StochasticDelayLineGranulator& interonsetTime( double min, double max ){ // seconds
        scheduler_.sequenceStrategy.interonsetTime( min*sr_, max*sr_ );
        return *this;
    }

    StochasticDelayLineGranulator& grainDuration( double min, double max ){ // seconds
        scheduler_.sequenceStrategy.duration( min*sr_, max*sr_ );
        return *this;
    }

    StochasticDelayLineGranulator& delayTime( double min, double max ){ // seconds
        scheduler_.sourceEssence.delaySamples( min*sr_, max*sr_ );
        return *this;
    }

    StochasticDelayLineGranulator& playbackRate( double min, double max ){
        scheduler_.sourceEssence.playbackRate( min, max );
        return *this;
    }

    StochasticDelayLineGranulator& amplitude( double min, double max ){
        scheduler_.envelopeEssence.amplitude( min, max );
        return *this;
    }

    StochasticDelayLineGranulator& sustain( double min, double max ){ // 0 to 1
        scheduler_.envelopeEssence.sustain( min, max );
        return *this;
    }

    StochasticDelayLineGranulator& skew( double min, double max ){ // -1 to 1
        scheduler_.envelopeEssence.skew( (min+1.)*.5, (max+1.)*.5 );
        return *this;
    }

    StochasticDelayLineGranulator& feedback( double x ){
        feedback_ = x;
        return *this;
    }

    void synthesize( float *output, float *input, size_t length ){
       
        //for( int i=0; i<length; ++i ){
        //    output[i] = delayLine_.tap( 10900. );
        //    delayLine_.write( input[i] + output[i] * .6 );
        //}

        scheduler_.synthesize( output, length );

        for( size_t i=0; i<length; ++i ){
            // smallNoise_ stops denormal problems on i86
            delayLine_.write( dcBlocker_(   smallNoise_.generate() +
                                            input[i] +
                                            output[i] * feedback_ ) );
        }
    }

private:
    scheduler_type scheduler_;
    DelayLine delayLine_;
    DCBlocker dcBlocker_;
    RangedDoubleRandomNumberGenerator smallNoise_;
    double sr_;
    float feedback_;
};                       

#endif /* INCLUDED_STOCHASTICDELAYLINEGRANULATOR_H */
