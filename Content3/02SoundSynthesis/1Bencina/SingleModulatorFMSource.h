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

#ifndef INCLUDED_SINGLEMODULATORFMSOURCE_H
#define INCLUDED_SINGLEMODULATORFMSOURCE_H

#include <ctype>

#include "SinusoidalOscillator.h"
#include "SinusoidalFMOscillator.h"

/*
    FMSource models GrainSourceT (see Grain.h)

    requires

    concept SingleModulatorFMEssenceT{
        double sampleRate() const;
        float modulatorFrequency() const;
        float carrierFrequency() const;
        float modulationIndex() const;
    }
*/

template< class SingleModulatorFMEssenceT >
class SingleModulatorFMSource{
public:
    typedef SingleModulatorFMEssenceT essence_type;
     
    SingleModulatorFMSource( size_t, size_t, const essence_type& essence )
        : modulationIndex_( essence.modulationIndex() ) {
        modulator_.frequency( essence.modulatorFrequency(), essence.sampleRate() );
        carrier_.frequency( essence.carrierFrequency(), essence.sampleRate() );
    }

    float operator()( float envelopeValue ){
        return envelopeValue * carrier_( *++modulator_ * modulationIndex_ );
    }

    size_t nextBoundary( size_t maximum ) const { return maximum; }
    
    void checkBoundary( size_t ) {}

private:
    SinusoidalOscillator modulator_;
    SinusoidalFMOscillator carrier_;
    float modulationIndex_;
};

#endif /* INCLUDED_SINGLEMODULATORFMSOURCE_H */
