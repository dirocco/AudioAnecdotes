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

#ifndef INCLUDED_BASICSAMPLERATEFREQUENCYPHASEESSENCE_H
#define INCLUDED_BASICSAMPLERATEFREQUENCYPHASEESSENCE_H

class BasicSampleRateFrequencyPhaseEssence{
public:
    BasicSampleRateFrequencyPhaseEssence( double sampleRate=44100., float frequency=440., double phase=0. )
        : sampleRate_( sampleRate ), frequency_(frequency), phase_(phase) {}

    BasicSampleRateFrequencyPhaseEssence& sampleRate( double x )
        { sampleRate_ = x; return *this; }
        
    BasicSampleRateFrequencyPhaseEssence& frequency( float x )
        { frequency_ = x; return *this; }

    BasicSampleRateFrequencyPhaseEssence& phase( float x )
        { phase_ = x; return *this; }


    double sampleRate() const { return sampleRate_; }
    double frequency() const { return frequency_; }
    double phase() const { return phase_; }
    
private:
    double sampleRate_;
    float frequency_;
    float phase_;
};

#endif /* INCLUDED_BASICSAMPLERATEFREQUENCYPHASEESSENCE_H */
