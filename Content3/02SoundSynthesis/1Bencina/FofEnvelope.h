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

#ifndef INCLUDED_FOFENVELOPE_H
#define INCLUDED_FOFENVELOPE_H

#include <math>

#include "RaisedCosineBellEnvelope.h"
#include "ExponentialEnvelope.h"
#include "EnvelopeComposer.h"


/*  This file is used by FofGenerator.h to perform FOF synthesis.

    FofEnvelopeEssence models:
    
    concept AmplitudeAttackSustainReleaseEssenceT{
        void getAmplitudeAttackSustainRelease( size_t duration, float&, size_t&,
                     size_t&, size_t& );
    };

    and
    
    concept AmplitudeAlphaEssenceT{
        float amplitude() const;
        float alpha() const;
    };
*/

class FofEnvelopeEssence{
public:
    FofEnvelopeEssence() {}
    
    FofEnvelopeEssence& amplitude( float x ) { amplitude_ = x; return *this; }
    FofEnvelopeEssence& bandwidth( double x ) { bandwidth_ = x; return *this; }   // -3db bandwidth
    FofEnvelopeEssence& skirtWidth( double x ) { skirtWidth_ = x; return *this; }
    FofEnvelopeEssence& sampleRate( double x ) { sr_ = x; return *this; }

    // raised cosine envelope arguments
    void makeAmplitudeAttackSustainRelease( size_t duration,
            float& amplitude, size_t& attack, size_t& sustain, size_t& release ) const {

        amplitude = 1.;

        // we make release == attack - 1, if there is not enough duration
        // then attack is shortened to compensate.
        // note that this is just a heuristic for smoothing the envelope tail
        // according to duration and has no other significance.

        attack = sr_ / skirtWidth_;;
        if( duration < attack * 2 )
            attack = duration *.5;

        release = attack - 1;
        sustain = duration - (attack + release);
    }

    // exponential envelope arguments
    float amplitude() const { return amplitude_; }
    float alpha() const { return bandwidth_ / (sr_ * M_PI); }


private:
    float amplitude_;
    double bandwidth_;
    double skirtWidth_;
    double sr_;
};


/*
    FofEnvelope models GrainEnvelopeT (see Grain.h)
*/

class FofEnvelope : public EnvelopeComposer<
        RaisedCosineBellEnvelope<FofEnvelopeEssence>,
        ExponentialEnvelope<FofEnvelopeEssence> >{
public:
    typedef FofEnvelopeEssence essence_type;
    
    FofEnvelope( size_t duration, const essence_type& essence )
        : EnvelopeComposer<RaisedCosineBellEnvelope<FofEnvelopeEssence>,
                ExponentialEnvelope<FofEnvelopeEssence> >
            ( duration, essence, essence ) {}
};

#endif /* INCLUDED_FOFENVELOPE_H */
