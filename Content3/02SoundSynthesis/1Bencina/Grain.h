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

#ifndef INCLUDED_GRAIN_H
#define INCLUDED_GRAIN_H

/*
    Grain models concept GrainT (see Scheduler.h)

    and takes the following type arguments:

    concept GrainSourceT {
        type essence_type;
        GrainSourceT( size_t vectorInskip, size_t duration, GrainSourceT::essence_type& essence );
        float operator()( float envelopeValue );    // returns source scaled by envelope value
        size_t nextBoundary( size_t maximum );
        void checkBoundary( size_t samplesPassed );
    }

    concept GrainEnvelopeT {
        type essence_type;
        EnvelopeT( size_t duration, GrainEnvelopeT::essence_type& essence );
        void advance();
        size_t nextBoundary( size_t maximum );
        void checkBoundary( size_t samplesPassed );
        bool atEnd();
        float operator*();
    }
*/

template< typename SourceT, typename SourceEssenceT,
            typename GrainEnvelopeT, typename GrainEnvelopeEssenceT >
class Grain{
public:
	typedef SourceT source_type;
    typedef SourceEssenceT source_essence_type;
	typedef GrainEnvelopeT envelope_type;
    typedef GrainEnvelopeEssenceT envelope_essence_type;

    Grain( size_t vectorInskip, size_t duration, source_essence_type& sourceEssence,
            envelope_essence_type& envelopeEssence )
        : source_( vectorInskip, duration, sourceEssence )
        , envelope_( duration, envelopeEssence ) {}

	void synthesize( float *output, size_t length ) {
        size_t remaining = length;
		do{
			size_t nextBoundary =
                envelope_.nextBoundary( source_.nextBoundary( remaining ) );

            float *end = output + nextBoundary;
            do{
                envelope_.advance();
				*output++ += source_( *envelope_ );
            }while( output < end );
            
			envelope_.checkBoundary( nextBoundary );
			source_.checkBoundary ( nextBoundary );
            remaining -= nextBoundary;
	
		}while( remaining > 0 && !envelope_.atEnd() );
	}
	
	bool atEnd() const { return envelope_.atEnd(); }

private:
    source_type source_;
    envelope_type envelope_;
};

#endif /* INCLUDED_GRAIN_H */
