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

#ifndef INCLUDED_HACKEDUPPARAMETERSEQUENCER_H
#define INCLUDED_HACKEDUPPARAMETERSEQUENCER_H

#include <iostreams>
#include <fstream>
#include <limits>
#include <string>
#include <map>
#include <vector>

/*
    HackedUpParametersSequencer is a quick-and-dirty class which
    can load time varying parameter values from a conficuration file, and
    play them back over time. The format of the configuration file is as
    follows:

        # indicates a single line comment

        parameter values are provided using name=value pairs, one per line

        there are three different types of parameters
            aStringParam = "a string value"
            aFloatingPointParam = 1.123
            aRangeParam = 123,456 #ie min,max

    a configuration file may contain multiple "segments" delimited by
    lines starting with %.

        % indicates the end of a segment

        the special parameter segment.duration sets
        the duration of the current segment

        the special parameter segment.annotation specifies a string
        which is written to cout at the beginning of that segment

    when a sequence is played back, all parameter values are ramped from
    one segment to the next over the duration of the segment. If a parameter
    value is not specified for a partictlar segment, then it holds the
    most recently supplied value for that parameter.
*/

class HackedUpParametersSequencer{
    class ParameterController{
    protected:
        std::string name_;
        double min_, max_;
        double clampValue( double value ){
            bool clamped = false;
            if( value > max_ ){
                value = max_;
                clamped = true;
            }else if( value < min_ ){
                value = min_;
                clamped = true;
            }
            if( clamped ){
                std::cerr << "Warning: Value supplied for '" << name_ << "' is out of range, clamping to " << value << "\n";
            }
            return value;
        }
    public:
        ParameterController( const std::string& name, double min=0., double max=0. )
            : name_( name ), min_(min), max_(max) {}
        virtual ~ParameterController() {}
        virtual void readParameter( std::istream& is ) = 0;
        virtual void pushParameter() = 0;
        virtual void beginSegment( unsigned long i ) = 0;
        virtual void updateSegment( unsigned long i, float fraction ) = 0;
        virtual void print() const = 0;
    };

    template< class T >
    class SingleValueParameterController: public ParameterController{
        T& target_;
        T& (T::*method_)( double );
        double input_;
        std::vector<double> segments_;
    public:
        SingleValueParameterController( const std::string& name, double min, double max, T& target, T& (T::*method)( double ) )
            : ParameterController( name, min, max ), target_( target ), method_( method ), input_(min){}
        virtual ~SingleValueParameterController() {}
        virtual void readParameter( std::istream& is ){
            input_=min_;
            is >> input_;
            input_ = clampValue( input_ );
        }
        virtual void pushParameter(){
            (target_.*method_)( input_ );
            segments_.push_back( input_ );
        }
        virtual void beginSegment( unsigned long i ){
            (target_.*method_)( segments_[i] );
        }
        virtual void updateSegment( unsigned long i, float fraction ){
            (target_.*method_)( segments_[i] + (segments_[i+1]-segments_[i])*fraction );
        }
        virtual void print() const {
            std::cerr << "\t" << name_ << " : value, min:" << min_ << " max:" << max_ << "\n";
        }
    };

    template< class T >
    class RangeValueParameterController : public ParameterController{
        T& target_;
        T& (T::*method_)( double, double );
        double inputMin_, inputMax_;
        std::vector<std::pair<double,double> > segments_;
    public:
        RangeValueParameterController( const std::string& name, double min, double max, T& target, T& (T::*method)( double, double ) )
            : ParameterController( name, min, max ), target_( target ), method_( method ), inputMin_(min), inputMax_(max) {}
        virtual ~RangeValueParameterController() {}
        virtual void readParameter( std::istream& is ){
            inputMin_=min_;
            is >> inputMin_;
            inputMin_ = clampValue( inputMin_ );

            inputMax_=inputMax_;

            char c;
            is >> c;
            if( c != ',' ){
                std::cerr << "Error: '" << name_ << "' is a range parameter and should be assigned min,max\n";
            }else{
                is >> inputMax_;
                inputMax_ = clampValue( inputMax_ );
            }
        }
        virtual void pushParameter(){
            (target_.*method_)( inputMin_, inputMax_ );
            segments_.push_back( std::pair<double,double>(inputMin_,inputMax_)  );
        }
        virtual void beginSegment( unsigned long i ){
            (target_.*method_)( segments_[i].first, segments_[i].second );
        }
        virtual void updateSegment( unsigned long i, float fraction ){
            std::pair<double,double> a(segments_[i]), b(segments_[i+1]);

            double min = a.first + (b.first-a.first)*fraction;
            double max = a.second + (b.second-a.second)*fraction;

            (target_.*method_)( min, max );
        }
        virtual void print() const {
            std::cerr << "\t" << name_ << " : range, min:" << min_ << " max:" << max_ << "\n";
        }
    };

    template< class T >
    class StringParameterController: public ParameterController{
        T& target_;
        T& (T::*method_)( const char* );
        std::string input_;
        std::vector<std::string> segments_;
    public:
        StringParameterController( const std::string& name, T& target, T& (T::*method)( const char * ) )
            : ParameterController( name ), target_( target ), method_( method ){}
        virtual ~StringParameterController() {}
        virtual void readParameter( std::istream& is ){
            char c;
            is >> c;
            if( c != '"' )
                std::cerr << "Error: expected quoted string while reading parameter '" << name_ << "'\n";

            std::string s;
            is.get(c);
            while( is && c != '"' && c != '\n' ){
                s += c;
                is.get(c);
            }

            if( c == '\n' )
                is.putback(c);

            input_ = s;
        }
        virtual void pushParameter(){
            (target_.*method_)( input_.c_str() );
            segments_.push_back( input_ );
            input_.clear();
        }
        virtual void beginSegment( unsigned long i ){
            (target_.*method_)( segments_[i].c_str() );
        }
        virtual void updateSegment( unsigned long, float ){}

        virtual void print() const {
            std::cerr << "\t" << name_ << " : string\n";
        }
    };
    
    typedef std::map< std::string, ParameterController* > controller_map_type;
    controller_map_type controllersMap_;
    typedef std::vector< ParameterController* > controller_vector_type;
    controller_vector_type controllers_;

    double duration_, segmentDuration_;
    std::string segmentAnnotation_;
    unsigned long segmentCount_;
    unsigned long currentSegmentNumber_;
    double currentSegmentInskip_, currentSegmentDuration_;

    HackedUpParametersSequencer& segmentDuration( double x ){
        segmentDuration_ = x;
        return *this;
    }

    HackedUpParametersSequencer& segmentAnnotation( const char *s ){
        segmentAnnotation_ = s;
        return *this;
    }

    void pushSegment(){
        for( controller_vector_type::iterator i=controllers_.begin(); i != controllers_.end(); ++i )
            (*i)->pushParameter();
            
        duration_ += segmentDuration_;
        segmentCount_++;
        segmentDuration_ = 0;
        segmentAnnotation_.clear();
    }

    void beginSegment( unsigned long segmentNumber ){
        for( controller_vector_type::iterator i=controllers_.begin(); i != controllers_.end(); ++i )
            (*i)->beginSegment(segmentNumber);
        currentSegmentDuration_ = segmentDuration_;
        if( !segmentAnnotation_.empty() )
            std::cout << segmentAnnotation_ << '\n';
    }

    void updateSegment( unsigned long segmentNumber, float fraction ){
        for( controller_vector_type::iterator i=controllers_.begin(); i != controllers_.end(); ++i )
            (*i)->updateSegment(segmentNumber, fraction);
    }

public:

    HackedUpParametersSequencer() : duration_( 0 ), segmentCount_(0) {
        bind( "segment.duration", 0., 1000000., *this, &HackedUpParametersSequencer::segmentDuration );
        bind( "segment.annotation", *this, &HackedUpParametersSequencer::segmentAnnotation );
    }

    ~HackedUpParametersSequencer(){
        while( !controllers_.empty() ){
            delete controllers_.back();
            controllers_.pop_back();
        }
    }

    template< class T >
    HackedUpParametersSequencer& bind( const std::string& name, double min, double max, T& target, T& (T::*method)( double ) ){
        ParameterController *pb = new SingleValueParameterController<T>( name, min, max, target, method );
        controllersMap_.insert( make_pair( name, pb ) );
        controllers_.push_back( pb );
        return *this;
    }

    template< class T >
    HackedUpParametersSequencer& bind( const std::string& name, double min, double max, T& target, T& (T::*method)( double, double ) ){
        ParameterController *pb = new RangeValueParameterController<T>( name, min, max, target, method );
        controllersMap_.insert( make_pair( name, pb ) );
        controllers_.push_back( pb );
        return *this;
    }

    template< class T >
    HackedUpParametersSequencer& bind( const std::string& name, T& target, T& (T::*method)( const char * ) ){
        ParameterController *pb = new StringParameterController<T>( name, target, method );
        controllersMap_.insert( make_pair( name, pb ) );
        controllers_.push_back( pb );
        return *this;
    }

    void printParameters(){
        for( controller_vector_type::iterator i=controllers_.begin(); i != controllers_.end(); ++i )
            (*i)->print();
    }
    
    double duration() const { return duration_; }
    
    bool read( const char *fileName ){
        duration_ = 0;
        segmentDuration_ = 0;
        segmentAnnotation_.clear();
        
        std::ifstream f( fileName );
        if( !f ){
            std::cerr << "Error: could not open data file '" << fileName << "'\n";
            return false;
        }

        char c;
        f >> c;
        while( f.good() ){
            if( c != '#' ){
                if( c == '%' ){
                    // end of section;
                    pushSegment();
                }else{
                    std::string parameterName;

                    while( f.good() && !(c == ' ' || c == '\t' || c == '\n' || c == '=') ){
                        parameterName += c;
                        f.get( c );
                    }

                    if( c == '\n' ){
                        std::cerr << "Error: Unexpected end of line\n";
                    }else{
                        if( c == ' ' || c == '\t' ){
                            f >> c;
                            if( c != '=' ){
                                std::cerr << "Error: Expected '='\n";
                            }
                        }

                        controller_map_type::iterator i = controllersMap_.find( parameterName );
                        if( i != controllersMap_.end() ){
                            i->second->readParameter( f );
                        }else{
                            std::cerr << "Warning: Unknown parameter '" << parameterName << "'\n";
                        }
                    }
                }
            }
            f.ignore( INT_MAX, '\n' );
            f >> c;
        }

        pushSegment();

        return true;
    }
    
    void beginPlayback(){
        assert( segmentCount_ != 0 );
        currentSegmentNumber_ = 0;
        currentSegmentInskip_ = 0;
        beginSegment(0);
    }

    void update( double timeElapsedSincePreviousCall ){
        if( currentSegmentNumber_ < (segmentCount_-1) ){
            currentSegmentInskip_ += timeElapsedSincePreviousCall;
            if( currentSegmentInskip_ >= currentSegmentDuration_ ){
                updateSegment( currentSegmentNumber_, 1. );
                currentSegmentNumber_++;
                if( currentSegmentNumber_ < segmentCount_ ){
                    currentSegmentInskip_ = 0.; // simple, but drops time at segment boundaries
                    beginSegment(currentSegmentNumber_);
                }
            }else{
                updateSegment( currentSegmentNumber_, currentSegmentInskip_ / currentSegmentDuration_ );
            }
        }
    }
};

#endif /* INCLUDED_HACKEDUPPARAMETERSEQUENCER_H */