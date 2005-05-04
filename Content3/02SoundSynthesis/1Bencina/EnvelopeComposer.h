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

#ifndef INCLUDED_ENVELOPECOMPOSER_H
#define INCLUDED_ENVELOPECOMPOSER_H

#include <functional>

/*
    EnvelopeComposer aggregates two envelopes (EnvelopeT1 & EnvelopeT2) into
    a single envelope type. The value of an instance of EnvelopeComposer is
    calculated by combining the values of the two contained envelopes using
    the binary operator supplied as parameter BinOp.

    EnvelopeComposer almost models GrainEnvelopeT (see Grain.h)
    the exception being that it does not define an essence_type.

    It is intended that users will subclass EnvelopeComposer and provide
    the necessary adapter code to convert one essence into the two
    required by EnvelopeComposer's constructor
*/         

template< class EnvelopeT1, class EnvelopeT2, class BinOp=std::multiplies<float> >
class EnvelopeComposer{
public:
    typedef EnvelopeT1 envelope1_type;
    typedef EnvelopeT2 envelope2_type;
    typedef BinOp composition_type;
    
    EnvelopeComposer( size_t duration,
                        const typename envelope1_type::essence_type& e1Essence,
                        const typename envelope2_type::essence_type& e2Essence )
        : e1_( duration, e1Essence ), e2_( duration, e2Essence ) {}


    float operator*() const {
        return composition_type()( *e1_, *e2_ );
    }
    
    EnvelopeComposer& advance(){
        e1_.advance();
        e2_.advance();
        return *this;
    }

    size_t nextBoundary( size_t maximum ) const {
        return e2_.nextBoundary( e1_.nextBoundary( maximum ) );
    }
    
    void checkBoundary( size_t samplesPassed ) {
        e1_.checkBoundary( samplesPassed );
        e2_.checkBoundary( samplesPassed );
    }
    
    bool atEnd() const { return e1_.atEnd() || e2_.atEnd(); }

private:
    envelope1_type e1_;
    envelope2_type e2_;
};

#endif /* INCLUDED_ENVELOPECOMPOSER_H */
