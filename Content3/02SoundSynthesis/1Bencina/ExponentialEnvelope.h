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

#ifndef INCLUDED_EXPONENTIALENVELOPE_H
#define INCLUDED_EXPONENTIALENVELOPE_H

#include <cmath>

/*
    ExponentialEnvelope models GrainEnvelopeT (see Grain.h)

    requires:

    concept AmplitudeAlphaEssenceT{
        float amplitude() const;
        float alpha() const;
    };
*/

template< class AmplitudeAlphaEssenceT >
class ExponentialEnvelope{
public:
    typedef AmplitudeAlphaEssenceT essence_type;

    ExponentialEnvelope( size_t, const essence_type& essence ) {
        value_ = essence.amplitude();
        c_ = std::exp( -essence.alpha() );
    }

    float operator*() const {
        return value_;
    }
    
    ExponentialEnvelope& advance(){
        value_ *= c_;
        return *this;
    }

    size_t nextBoundary( size_t maximum ) const {
        return maximum;
    }
    
    void checkBoundary( size_t ) {
    }

    /*
        ExponentialEnvelope indicates atEnd() when it's amplitude falls
        below a (currently hardwired) threshold.
    */
    
    bool atEnd() const { return value_ < .000001; /*return false;*/ }

private:
    float value_, c_;
};

#endif /* INCLUDED_EXPONENTIALENVELOPE_H */
