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

#ifndef INCLUDED_RANDOM_H
#define INCLUDED_RANDOM_H

/*
    31 bit Park-Miller linear congruential PRNG
    based on a public domain implementation by Ray Gardner
    available at http://www.snippets.org/snippets/RG_RAND.C
*/

class PRNG_31bit{
public:
    static void seed( long seed ) {
        seed_ = seed ? (seed & m) : 1;  // nonzero seed
    }
    
    static long generate() {        
        unsigned long lo = a * (long)(seed_ & 0xFFFF);
        unsigned long hi = a * (long)((unsigned long)seed_ >> 16);
        lo += (hi & 0x7FFF) << 16;
        if (lo > m) {
              lo &= m;
              ++lo;
        }
        lo += hi >> 15;
        if (lo > m) {
              lo &= m;
              ++lo;
        }
        seed_ = (long)lo;
        return seed_;
    }
    
private:
    static const int a;
    static const unsigned long m;
    static long seed_;
};


/*
    UnitDoubleRandomNumberGenerator returns random numbers from 0. to 1.
*/
class UnitDoubleRandomNumberGenerator{
public:
    double generate() { return unitScale * (double)PRNG_31bit::generate(); }
    static const double unitScale;
};


/*
    RangedDoubleRandomNumberGenerator returns random numbers in [min, max]
*/
class RangedDoubleRandomNumberGenerator{
public:
    RangedDoubleRandomNumberGenerator() : min_(0.), scale_(1.) {}
    RangedDoubleRandomNumberGenerator( double min, double max )
        { range( min, max ); }

    void range( double min, double max ) {
        min_ = min;
        scale_ = (max - min) * UnitDoubleRandomNumberGenerator::unitScale;
    }
    
    double generate() { return min_ + scale_ * (double)PRNG_31bit::generate(); }
private:
    double min_;
    double scale_;
};


/*
    RangedLongRandomNumberGenerator returns random numbers in [min, max]
    (this could be improved by removing the use of doubles)
*/
class RangedLongRandomNumberGenerator{
public:
    RangedLongRandomNumberGenerator() { range( 0, 0x7FFFFFFF ); }
    RangedLongRandomNumberGenerator( long min, long max )
        { range( min, max ); }
        
    void range( long min, long max ) {
        min_ = min;
        scale_ = (max - min) * UnitDoubleRandomNumberGenerator::unitScale;
    }
    
    double generate()
        { return (long)(min_ + scale_ * (double)PRNG_31bit::generate()); }
private:
    long min_;
    double scale_;
};

#endif /* INCLUDED_RANDOM_H */
