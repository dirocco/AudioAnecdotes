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

#include <iostream>

// include everything for testing...
//#include "Everything.h"

#include "StochasticFMGranulator.h"
#include "StochasticStoredSampleGranulator.h"
#include "StochasticDelayLineGranulator.h"
#include "FofGenerator.h"

#include "HackedUpParameterSequencer.h"

#include "pablio.h"

template< class Source, class Sequencer >
void outputAudio( double sampleRate, double duration, Source& source, Sequencer& sequencer ){
    static const int length = 64;
    float output[length];

    double secondsPerVector = (double)length / sampleRate;
    sequencer.beginPlayback();

    PABLIO_Stream *stream;
    OpenAudioStream( &stream, sampleRate, paFloat32, PABLIO_WRITE | PABLIO_MONO );

    unsigned long count = (sampleRate / length) * duration;
    for( unsigned long i=0; i<count; ++i ){
        std::fill_n( output, length, 0.f );  // zero output
        source.synthesize( output, length );
        sequencer.update( secondsPerVector );
        WriteAudioStream( stream, output, length );
    }

    CloseAudioStream( stream );
}

template< class Filter, class Sequencer >
void filterAudio( double sampleRate, double duration, Filter& filter, Sequencer& sequencer ){
    static const int length = 64;
    float input[length], output[length];

    double secondsPerVector = (double)length / sampleRate;
    sequencer.beginPlayback();

    PABLIO_Stream *stream;
    OpenAudioStream( &stream, sampleRate, paFloat32, PABLIO_READ_WRITE | PABLIO_MONO );

    unsigned long count = (sampleRate / length) * duration;
    for( unsigned long i=0; i<count; ++i ){
        ReadAudioStream( stream, input, length );
        std::fill_n( output, length, 0.f );  // zero output
        filter.synthesize( output, input, length );
        sequencer.update( secondsPerVector );
        WriteAudioStream( stream, output, length );
    }

    CloseAudioStream( stream );
}


bool fm( const char *dataFileName )
{
    double sr = 44100.;
    int maxGrains = 100;
    StochasticFMGranulator granulator( maxGrains, sr );

    HackedUpParametersSequencer sequencer;
    sequencer
        .bind( "density", .1, 1000000., granulator, &StochasticFMGranulator::density )
        .bind( "grainDuration", .001, 1., granulator, &StochasticFMGranulator::grainDuration )
        .bind( "amplitude", 0., 1., granulator, &StochasticFMGranulator::amplitude )
        .bind( "carrierFrequency", 0., sr * .5, granulator, &StochasticFMGranulator::carrierFrequency )
        .bind( "modulatorFrequency", 0., sr * .5, granulator, &StochasticFMGranulator::modulatorFrequency )
        .bind( "modulationIndex", 0., 10., granulator, &StochasticFMGranulator::modulationIndex );

    if( sequencer.read( dataFileName ) ){
        outputAudio( sr, sequencer.duration(), granulator, sequencer );
        return true;
    }else{
        std::cerr << "Allowable parameters for data-file:\n";
        sequencer.printParameters();
        return false;
    }
}

bool dl( const char *dataFileName )
{
    double sr = 44100.;
    int maxGrains = 100;
    double maxDelaySeconds = 10;
    StochasticDelayLineGranulator granulator( maxGrains, maxDelaySeconds, sr );

    HackedUpParametersSequencer sequencer;
    sequencer
        .bind( "interonsetTime", .0001, 2., granulator, &StochasticDelayLineGranulator::interonsetTime )
        .bind( "grainDuration", .001, 1., granulator, &StochasticDelayLineGranulator::grainDuration )
        .bind( "delayTime", 0, maxDelaySeconds, granulator, &StochasticDelayLineGranulator::delayTime )
        .bind( "playbackRate", .1, 10., granulator, &StochasticDelayLineGranulator::playbackRate )
        .bind( "amplitude", 0., 1., granulator, &StochasticDelayLineGranulator::amplitude )
        .bind( "sustain", 0., 1., granulator, &StochasticDelayLineGranulator::sustain )
        .bind( "skew", -1., 1., granulator, &StochasticDelayLineGranulator::skew )
        .bind( "feedback", 0., .95, granulator, &StochasticDelayLineGranulator::feedback );

    if( sequencer.read( dataFileName ) ){
        filterAudio( sr, sequencer.duration(), granulator, sequencer );
        return true;
    }else{
        std::cerr << "Allowable parameters for data-file:\n";
        sequencer.printParameters();
        return false;
    }
}

bool sample( const char *dataFileName )
{
    double sr = 44100.;
    int maxGrains = 100;
    double maxSampleSeconds = 10;
    StochasticStoredSampleGranulator granulator( maxGrains, maxSampleSeconds, sr );

    HackedUpParametersSequencer sequencer;
    sequencer
        .bind( "sourceFile", granulator, &StochasticStoredSampleGranulator::sourceFile )
        .bind( "interonsetTime", .0001, 1., granulator, &StochasticStoredSampleGranulator::interonsetTime )
        .bind( "grainDuration", .001, 1., granulator, &StochasticStoredSampleGranulator::grainDuration )
        .bind( "inskipTime", 0., maxSampleSeconds, granulator, &StochasticStoredSampleGranulator::inskipTime )
        .bind( "playbackRate", 0.1, 10., granulator, &StochasticStoredSampleGranulator::playbackRate )
        .bind( "amplitude", 0., 1., granulator, &StochasticStoredSampleGranulator::amplitude )
        .bind( "sustain", 0., 1., granulator, &StochasticStoredSampleGranulator::sustain )
        .bind( "skew", -1., 1., granulator, &StochasticStoredSampleGranulator::skew );

    if( sequencer.read( dataFileName ) ){
        outputAudio( sr, sequencer.duration(), granulator, sequencer );
        return true;
    }else{
        std::cerr << "Allowable parameters for data-file:\n";
        sequencer.printParameters();
        return false;
    }
}

struct ThreeFofs{
    FofGenerator fof1, fof2, fof3;
    ThreeFofs( int maxGrains, double sr )
        : fof1( maxGrains ), fof2( maxGrains ), fof3( maxGrains ) {
        fof1.sampleRate(sr);
        fof2.sampleRate(sr);
        fof3.sampleRate(sr);
    }

    void synthesize( float *output, unsigned long length ){
        fof1.synthesize( output, length );
        fof2.synthesize( output, length );
        fof3.synthesize( output, length );
    }                                     
};

bool fof( const char *dataFileName )
{
    double sr = 44100.;
    int maxGrains = 10;
    ThreeFofs  fofs(maxGrains, sr);

    HackedUpParametersSequencer sequencer;
    sequencer
        .bind( "fof1.fundamentalFrequency", 0, sr * .5, fofs.fof1, &FofGenerator::fundamentalFrequency )
        .bind( "fof1.formantCenterFrequency", 0, sr * .5, fofs.fof1, &FofGenerator::formantCenterFrequency )
        .bind( "fof1.formantBandwidth", 0., sr * .5, fofs.fof1, &FofGenerator::formantBandwidth )
        .bind( "fof1.formantSkirtWidth", 0., sr * .5, fofs.fof1, &FofGenerator::formantSkirtWidth )
        .bind( "fof1.formantAmplitude", 0., 1, fofs.fof1, &FofGenerator::formantAmplitude )
        .bind( "fof1.formantPhase", -M_PI, M_PI, fofs.fof1, &FofGenerator::formantPhase )

        .bind( "fof2.fundamentalFrequency", 0, sr * .5, fofs.fof2, &FofGenerator::fundamentalFrequency )
        .bind( "fof2.formantCenterFrequency", 0, sr * .5, fofs.fof2, &FofGenerator::formantCenterFrequency )
        .bind( "fof2.formantBandwidth", 0., sr * .5, fofs.fof2, &FofGenerator::formantBandwidth )
        .bind( "fof2.formantSkirtWidth", 0., sr * .5, fofs.fof2, &FofGenerator::formantSkirtWidth )
        .bind( "fof2.formantAmplitude", 0., 1, fofs.fof2, &FofGenerator::formantAmplitude )
        .bind( "fof2.formantPhase", -M_PI, M_PI, fofs.fof2, &FofGenerator::formantPhase )

        .bind( "fof3.fundamentalFrequency", 0, sr * .5, fofs.fof3, &FofGenerator::fundamentalFrequency )
        .bind( "fof3.formantCenterFrequency", 0, sr * .5, fofs.fof3, &FofGenerator::formantCenterFrequency )
        .bind( "fof3.formantBandwidth", 0., sr * .5, fofs.fof3, &FofGenerator::formantBandwidth )
        .bind( "fof3.formantSkirtWidth", 0., sr * .5, fofs.fof3, &FofGenerator::formantSkirtWidth )
        .bind( "fof3.formantAmplitude", 0., 1, fofs.fof3, &FofGenerator::formantAmplitude )
        .bind( "fof3.formantPhase", -M_PI, M_PI, fofs.fof3, &FofGenerator::formantPhase );


    if( sequencer.read( dataFileName ) ){
        outputAudio( sr, sequencer.duration(), fofs, sequencer );
        return true;
    }else{
        std::cerr << "Allowable parameters for data-file:\n";
        sequencer.printParameters();
        return false;
    }
}

int main(int argc, char* argv[])
{
    bool printUsage = false;
    std::string whichGranulator;
    if( argc > 1 ){
        whichGranulator = argv[1];
    }else{
        printUsage |= true;
        whichGranulator = "fm";
        std::cerr << "Using default granulator (" << whichGranulator << ")\n";
    }

    std::string dataFile;
    if( argc > 2 ){
        dataFile = argv[2];
    }else{
        printUsage |= true;
        dataFile = whichGranulator + ".data";
        std::cerr << "Using default data file " << dataFile << '\n';
    }

    if( whichGranulator == "fm" ){
        printUsage = !fm( dataFile.c_str() );
    }else if( whichGranulator == "sample" ){
        printUsage = !sample( dataFile.c_str() );
    }else if( whichGranulator == "dl" ){
        printUsage = !dl( dataFile.c_str() );
    }else if( whichGranulator == "fof" ){
        printUsage = !fof( dataFile.c_str() );
    }else{
        printUsage |= true;
        std::cerr << "Error: unknown granulator '" << whichGranulator << "'\n";
    }

    if( printUsage )
        std::cerr << "Usage: " << argv[0] << " {fm,sample,dl,fof} [data-file]\n";
}
