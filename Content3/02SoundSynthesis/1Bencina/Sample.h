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

#ifndef INCLUDED_SAMPLE_H
#define INCLUDED_SAMPLE_H

#include <cstdio>

class Sample{
public:
    Sample( size_t length ) : length_( length ), data_( new short[ length + 1 ] ) {}
    ~Sample() { delete [] data_; }

    bool readRawPcm( const char* fileName ){ // read mono raw PCM shorts, not endian portable
        bool result = false;
        size_t shortsRead = 0;
        std::FILE *fp = std::fopen( fileName, "rb" );
        if( fp ){
            shortsRead = std::fread( data_, sizeof(short), length_, fp );
            std::fclose(fp);
            result = true;
        }
        for( size_t i=shortsRead; i < length_; ++i )
            data_[i] = 0;
        data_[length_] = data_[length_-1];

        return result;
    }

    size_t length() const { return length_; }
    short* data() const { return data_; }

    float linearInterpolatedLookup( double index ) const { // returns a normalized signal
        long i = index;
        float fraction = index - i;
        return ( data_[i] + (data_[i+1]-data_[i]) * fraction ) * 0.000030517578125;
    }

    class LinearInterpolatedIterator{
    public:
        LinearInterpolatedIterator( const Sample& sample,
                        double initialIndex, double increment )
            : sample_( sample )
            , index_( initialIndex )
            , increment_( increment ) {}


        LinearInterpolatedIterator& operator++() {
            index_ += increment_;
            return *this;
        }

        float operator*() const { return sample_.linearInterpolatedLookup( index_ ); }
    private:
        const Sample& sample_;
        double index_;
        double increment_;
    };
    
private:
    size_t length_;
    short *data_;
};

#endif /* INCLUDED_SAMPLE_H */
