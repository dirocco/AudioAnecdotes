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

#ifndef INCLUDED_STOCHASTICSTOREDSAMPLEGRANULATOR_H
#define INCLUDED_STOCHASTICSTOREDSAMPLEGRANULATOR_H

#include "Sample.h"

#include "SamplePlaybackSource.h"
#include "StochasticInskipSamplesPlaybackRateEssence.h"

#include "RaisedCosineBellEnvelope.h"
#include "StochasticAmplitudeSustainSkewEssence.h"

#include "StochasticInteronsetTimeSequenceStrategy.h"

#include "Grain.h"
#include "Scheduler.h"

                         
class StochasticStoredSampleGranulator{
public:
    typedef Grain<  SamplePlaybackSource<StochasticInskipSamplesPlaybackRateEssence>, StochasticInskipSamplesPlaybackRateEssence,
                    RaisedCosineBellEnvelope<StochasticAmplitudeSustainSkewEssence>, StochasticAmplitudeSustainSkewEssence > grain_type;

    typedef Scheduler< StochasticInteronsetTimeSequenceStrategy, grain_type > scheduler_type;


    StochasticStoredSampleGranulator( int maxGrainPolyphony,
                                    double maxSampleSeconds,
                                    double sampleRate )
        : scheduler_( maxGrainPolyphony )
        , sample_( maxSampleSeconds * sampleRate ) // could be longer to account for playback rates > 1.
        , sr_( sampleRate ) {

        scheduler_.sourceEssence.sample( &sample_ );
    }

    StochasticStoredSampleGranulator& sampleRate( double x ){
        sr_ = x;
        return *this;
    }

    StochasticStoredSampleGranulator& sourceFile( const char *raw16BitPcmfileName ){
        if( raw16BitPcmfileName && raw16BitPcmfileName[0] != '\0' )
            sample_.readRawPcm( raw16BitPcmfileName );
        return *this;
    }

    StochasticStoredSampleGranulator& interonsetTime( double min, double max ){ // seconds
        scheduler_.sequenceStrategy.interonsetTime( min*sr_, max*sr_ );
        return *this;
    }

    StochasticStoredSampleGranulator& grainDuration( double min, double max ){ // seconds
        scheduler_.sequenceStrategy.duration( min*sr_, max*sr_ );
        return *this;
    }

    StochasticStoredSampleGranulator& inskipTime( double min, double max ){ // seconds
        scheduler_.sourceEssence.inskipSamples( min*sr_, max*sr_ );
        return *this;
    }

    StochasticStoredSampleGranulator& playbackRate( double min, double max ){
        scheduler_.sourceEssence.playbackRate( min, max );
        return *this;
    }

    StochasticStoredSampleGranulator& amplitude( double min, double max ){
        scheduler_.envelopeEssence.amplitude( min, max );
        return *this;
    }

    StochasticStoredSampleGranulator& sustain( double min, double max ){ // 0 to 1
        scheduler_.envelopeEssence.sustain( min, max );
        return *this;
    }

    StochasticStoredSampleGranulator& skew( double min, double max ){ // -1 to 1
        scheduler_.envelopeEssence.skew( (min+1.)*.5, (max+1.)*.5 );
        return *this;
    }

    void synthesize( float *output, size_t length ){
        scheduler_.synthesize( output, length );
    }

private:
    scheduler_type scheduler_;
    Sample sample_;
    double sr_;
};                       

#endif /* INCLUDED_STOCHASTICSTOREDSAMPLEGRANULATOR_H */
