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

#ifndef INCLUDED_DELAYLINE_H
#define INCLUDED_DELAYLINE_H

#include <cassert>
#include <math>

/*
    Not the most efficient DelayLine imaginable. The Tap could use
    the nextBoundary()/checkBoundary() mechanism for example.
*/

class DelayLine{
public:
	class FixedRateTap{
    public:
        FixedRateTap( DelayLine& delayLine, double initialDelaySamples, double playbackRate=1. )
            : delayLine_( delayLine )
            , readIndex_( makeReadIndex( delayLine, initialDelaySamples ) )
            , increment_( playbackRate ) {}
            
        FixedRateTap& operator++() {
            readIndex_ += increment_;
            if( readIndex_ >= delayLine_.bufferLength_ )
                readIndex_ -= delayLine_.bufferLength_;
            return *this;
        }

        float operator*() const {
            unsigned long i = std::floor( readIndex_ ); // use a more efficient alternative on windows
            float fraction = readIndex_ - i;
            return delayLine_.delayBuffer_[i] +
                (delayLine_.delayBuffer_[i+1]-delayLine_.delayBuffer_[i]) * fraction;
        }

    private:
        static double makeReadIndex( DelayLine& delayLine, double delaySamples ){
            double result = delayLine.writeIndex_ - delaySamples;
            if( result < 0 )
                result += delayLine.bufferLength_;
            return result;
        }
        DelayLine& delayLine_;
        double readIndex_;
        double increment_;
	};
    friend FixedRateTap;


	DelayLine( unsigned long maximumDelaySamples )
        : bufferLength_( maximumDelaySamples )
        , writeIndex_( 0 )
    {
        delayBuffer_ = new float[ bufferLength_ + 1 ];

        std::fill_n( delayBuffer_, bufferLength_ + 1, 0.f );
	}

	~DelayLine(){
        delete [] delayBuffer_;
	}

	void write( float input ){
        ++writeIndex_;
        delayBuffer_[ writeIndex_ ] = input;
        if( writeIndex_ == bufferLength_ ){
            // delayBuffer_[ bufferLength_ ] == delayBuffer_[0]
            // for easy linear interpolation

            writeIndex_ = 0;
            *delayBuffer_ = input;
        }
	}

    void write( float *input, size_t length ){ // optimize this
        for( size_t i=0; i<length; i++ )
            write( input[i] );
    }

private:
    unsigned long bufferLength_;
    unsigned long writeIndex_;
    float *delayBuffer_;
};

#endif /* INCLUDED_DELAYLINE_H */
