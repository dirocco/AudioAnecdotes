/*
 * Copyright (c) 1997-2001 Alexandros Eleftheriadis, Yuntai Kyong and 
 * Danny Hong
 *
 * This file is part of Flavor, developed at Columbia University
 * (www.ee.columbia.edu/flavor)
 *
 * Flavor is free software; you can redistribute it and/or modify
 * it under the terms of the Flavor Artistic License as described in
 * the file COPYING.txt. 
 *
 */

/* 
 * Authors:
 * Danny Hong <danny@ee.columbia.edu>
 *
 */


#include <stdio.h>

// You must include the run-time library before the Flavor-generated files
#include <flavor.h>

// Include the Flavor-generated files
#include "audio.h"


#include "pablio.h"


/****************************************************************
 * Extended precision IEEE floating-point conversion routine.
 ****************************************************************/

double ConvertFromIeeeExtended(ExtendedFloat *sampleRate)
{
    double f;

    if (sampleRate->exponent == 0 &&
        sampleRate->mantissa_hi == 0 && sampleRate->mantissa_lo == 0) {
        f = 0;
    }
    else {
        if (sampleRate->exponent == 0x7FFF) {    /* Infinity or NaN */
            f = HUGE_VAL;
        }
        else {
            int exp = sampleRate->exponent - 16383;

#define UnsignedToFloat(u) (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)

            f  = ldexp(UnsignedToFloat(sampleRate->mantissa_hi), exp-=31);
            f += ldexp(UnsignedToFloat(sampleRate->mantissa_lo), exp-=32);
        }
    }

    if (sampleRate->sign)
        return -f;
    else
        return f;
}


struct PCMsound {
    unsigned char bytesPerSample;	// both quantization and packing
    unsigned char samplesPerFrame;	// muliplexing (1 mono, 2 stereo, etc.)
    unsigned int  framesPerSecond;	// confusingly referred to (in other APIs)
									// as samplerate
    // describe the buffer
    unsigned long bytes;			// size of buffer in bytes
    void* data;
};

void play(PCMsound *sound)
{
    unsigned char *data = (unsigned char *) sound->data;
    PABLIO_Stream  *outStream;
    PaSampleFormat format;
    long flags = PABLIO_WRITE;
    int x;

    switch (sound->bytesPerSample) {
    case 1: format = paInt8;  break;
    case 2: format = paInt16; break;
    case 4: format = paInt32; break;
    }

    switch (sound->samplesPerFrame) {
    case 1: flags |= PABLIO_MONO;   break;
    case 2: flags |= PABLIO_STEREO; break;
    }

    OpenAudioStream(&outStream, sound->framesPerSecond, format, flags);

    WriteAudioStream(outStream, sound->data, sound->bytes / (sound->bytesPerSample*sound->samplesPerFrame)); 

    CloseAudioStream(outStream);
}


void usage(void)
{
    printf("Usage: play audio_file\n");
}


int main(int argc, char *argv[])
{
    char * option;

    if (argc != 2) {
        usage();
        return 1;
    }
    else {

        // Automatic detection
        Audio audio;
        Bitstream bs(argv[1], BS_INPUT);

        // Parse
#ifdef USE_EXCEPTION
        try {
            audio.get(bs);
        } catch(Bitstream::Error e) {
            // Catch exception from bitstream I/O library
            fprintf(stderr, "%s: Error: %s\n", argv[2], e.getmsg());
            exit(1);
        }
#else 
        audio.get(bs);
        if (bs.geterror() != E_NONE) {
            fprintf(stderr, "%s: Error: %s\n", argv[2], bs.getmsg());
            exit(1);
        }
#endif

        PCMsound sound;

        switch (audio.magic) {

        case 0x464F524D:    // "FORM"   -- AIFF/AIFC Format
            sound.bytesPerSample  = audio.aiff->ckCommon->sampleSize >> 3;
            sound.samplesPerFrame = audio.aiff->ckCommon->numChannels;
            sound.framesPerSecond = (unsigned int) ConvertFromIeeeExtended(audio.aiff->ckCommon->sampleRate);
            sound.bytes = audio.aiff->ckSound->ckSize;
            sound.data  = audio.aiff->ckSound->data;	// big-endian
            break;

        case 0x2E736E64:    // ".snd"   -- NeXT/Sun Format
            switch(audio.au->hd->dataFormat) {

/* Define the encoding fields */
#define AUDIO_FILE_ENCODING_MULAW_8   (1)   /* 8-bit ISDN u-law */  
#define AUDIO_FILE_ENCODING_LINEAR_8  (2)   /* 8-bit linear PCM */  
#define AUDIO_FILE_ENCODING_LINEAR_16 (3)   /* 16-bit linear PCM */  
#define AUDIO_FILE_ENCODING_LINEAR_24 (4)   /* 24-bit linear PCM */   
#define AUDIO_FILE_ENCODING_LINEAR_32 (5)   /* 32-bit linear PCM */   
#define AUDIO_FILE_ENCODING_FLOAT     (6)   /* 32-bit IEEE floating point */   
#define AUDIO_FILE_ENCODING_DOUBLE    (7)	/* 64-bit IEEE floating point */   

            case AUDIO_FILE_ENCODING_LINEAR_8:  sound.bytesPerSample = 1; break;
            case AUDIO_FILE_ENCODING_LINEAR_16: sound.bytesPerSample = 2; break;
            case AUDIO_FILE_ENCODING_LINEAR_32: sound.bytesPerSample = 4; break;
            }
            sound.samplesPerFrame = audio.au->hd->channelCount;
            sound.framesPerSecond = audio.au->hd->samplingRate;
            sound.bytes = audio.au->hd->dataSize;
            sound.data  = audio.au->data;				// big-endian
            break;

        case 0x52494646:    // "RIFF"   -- WAV Format

            sound.bytesPerSample  = audio.wav->ckFormat->bitsPerSample >> 3;
            sound.samplesPerFrame = audio.wav->ckFormat->channels;
            sound.framesPerSecond = audio.wav->ckFormat->samplesPerSec;
            sound.bytes = audio.wav->ckData->ckSize;
            sound.data  = audio.wav->ckData->data;		// little-endian
            break;

        default:            // Raw Format
            fprintf(stderr, "Unrecognized audio file format: 0x%08x\n", audio.magic);
            exit(1);
        }

        // printf("%d %d %d %d\n", sound.bytesPerSample,sound.samplesPerFrame,sound.framesPerSecond, sound.bytes);

        // pack the 4 bytes per sample data into the right number of bytes
        for (int i = 0,j = 0; i < sound.bytes; i += sound.bytesPerSample, j++) {
            void *data = (char *) sound.data + i;
            int sample = *((int  *) sound.data + j);
            if (sound.bytesPerSample == 1)
                *((char *) data) = (char) sample;
            else if (sound.bytesPerSample == 2)
                *((short *) data) = (short) sample;
            else if (sound.bytesPerSample == 4)
                *((int *) data) = (int) sample;
        }

        play(&sound);
    }

    // Done
    return 0;
}
