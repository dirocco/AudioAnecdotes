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

#ifndef INCLUDED_SCHEDULER_H
#define INCLUDED_SCHEDULER_H

#include <vector>
#include <algorithm>

/* Scheduler requires SequenceStrategyT and GrainT to respectively model
    the following concepts:

    concept SequenceStrategyT{
        size_t nextDuration();
        size_t nextInteronset();
    }

    concept GrainT{
        type source_essence_type;
        type envelope_essenceType;

        GrainT( vectorInskip, duration, envelopeEssence, sourceEssence );

        void synthesize( float *output, size_t length );
        bool atEnd();
    }
*/

template< class SequenceStrategyT, class GrainT >
class Scheduler{
public:
    typedef SequenceStrategyT sequence_strategy_type;
    typedef GrainT grain_type;
    typedef typename grain_type::source_essence_type source_essence_type;
	typedef typename grain_type::envelope_essence_type envelope_essence_type;


    Scheduler( int maxGrains )
        : nextOnset_( 0 )
        , activeGrainMask_( maxGrains, false )
        , grains_( reinterpret_cast<grain_type*>(operator new( maxGrains * sizeof( grain_type ) )) ) { }
        
    ~Scheduler() {
        grain_type *grain = grains_;
        for( std::vector<bool>::const_iterator i = activeGrainMask_.begin();
                i != activeGrainMask_.end(); ++i, ++grain ){

            if( *i )
                grain->~GrainT();
        }

        operator delete( grains_ );
    }

    int maxGrains() const { return activeGrainMask_.size(); }
    
    void synthesize( float *output, size_t length ) {
        unsigned long iNextOnset = std::floor(nextOnset_);

        GrainT *grain = grains_;
        std::vector<bool>::iterator grainIsActive = activeGrainMask_.begin();

        while( grainIsActive != activeGrainMask_.end() ){

            if( *grainIsActive ){
                grain->synthesize( output, length );
                if( grain->atEnd() ){
                    grain->~GrainT();
                    *grainIsActive = false;
                }
            }

            
            if( *grainIsActive == false && iNextOnset < length ){
                new (grain) GrainT( iNextOnset, sequenceStrategy.nextDuration(),
                                sourceEssence, envelopeEssence );

                grain->synthesize( output + iNextOnset, length - iNextOnset );
                
                nextOnset_ += sequenceStrategy.nextInteronset();
                iNextOnset = std::floor(nextOnset_);
                *grainIsActive = true;
            }

            grain++;
            grainIsActive++;
        }

        // discard onsets which couldn't be activated due to lack of polyphony
        while( iNextOnset < length ){
            nextOnset_ += sequenceStrategy.nextInteronset();
            iNextOnset = std::floor(nextOnset_);
        }
        
        nextOnset_ -= length;
    }

    sequence_strategy_type sequenceStrategy;

	source_essence_type sourceEssence;
	envelope_essence_type envelopeEssence;
    
private:
    double nextOnset_;
    std::vector<bool> activeGrainMask_;
    grain_type *grains_;
};

#endif /* INCLUDED_SCHEDULER_H */
