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

#ifndef INCLUDED_BASICAMPLITUDESKEWSUSTAINESSENCE_H
#define INCLUDED_BASICAMPLITUDESKEWSUSTAINESSENCE_H

/*
    BasicAmplitudeSkewSustainEssence models:
    
    concept AmplitudeAttackSustainReleaseEssenceT {
        makeAmplitudeAttackSustainRelease( size_t duration,
            float& amplitude, size_t& attack, size_t& sustain, size_t& release );
    }
*/

class BasicAmplitudeSustainSkewEssence{
public:
    BasicAmplitudeSustainSkewEssence() {}

    BasicAmplitudeSustainSkewEssence( float amplitude, float skew, float sustain )
        : amplitude_( amplitude )
        , skew_( skew )
        , sustain_( sustain ) {}

    BasicAmplitudeSustainSkewEssence& amplitude( float x )
        { amplitude_ = x; return *this; }
    BasicAmplitudeSustainSkewEssence& sustain( float x ){
        assert( x >= 0. && x <= 1. );
        sustain_ = x; return *this;
    }
    BasicAmplitudeSustainSkewEssence& skew( float x ){
        assert( x >= 0. && x <= 1. );
        skew_ = x; return *this;
    }

    void makeAmplitudeAttackSustainRelease( size_t duration,
            float& amplitude, size_t& attack, size_t& sustain, size_t& release ) const {

        amplitude = amplitude_;
        
        durationSustainSkewToAttackSustainRelease( duration, sustain_, skew_, attack, sustain, release );
    }

    static void durationSustainSkewToAttackSustainRelease( size_t duration, float sustainFraction, float skew,
            size_t& attack, size_t& sustain, size_t& release ){

        assert( duration >= 3 );
        size_t durationM3 = duration - 3;

        sustain = durationM3 * sustainFraction;
        attack = (durationM3 - sustain) * skew;
		release = durationM3 - attack - sustain;

        attack++;
		assert( attack > 0 && attack <= duration );

        sustain++;
        assert( sustain > 0 && sustain <= duration );

        release++;
        assert( release > 0 && release <= duration );
    }

protected:
    float amplitude_, sustain_, skew_;
};


class ClampedBasicAmplitudeSustainSkewEssence
    : private BasicAmplitudeSustainSkewEssence{
public:
    ClampedBasicAmplitudeSustainSkewEssence() {}

    ClampedBasicAmplitudeSustainSkewEssence( float amplitude, float sustain, float skew )
        : BasicAmplitudeSustainSkewEssence( amplitude, sustain, skew ) {}

    ClampedBasicAmplitudeSustainSkewEssence& amplitude( float x )
        { amplitude_ = x; return *this; }
    ClampedBasicAmplitudeSustainSkewEssence& sustain( float x )
        { sustain_ = clamp( x, 0.f, 1.f ); return *this; }
    ClampedBasicAmplitudeSustainSkewEssence& skew( float x )
        { skew_ = clamp( x, 0.f, 1.f ); return *this; }
        
    using BasicAmplitudeSustainSkewEssence::makeAmplitudeAttackSustainRelease;
    
private:
    static float clamp( float value, float min, float max ){
        if( value > max )
            return max;
        else if( value < min )
            return min;
        else
            return value;
    }
};

#endif /* INCLUDED_BASICAMPLITUDESKEWSUSTAINESSENCE_H */
