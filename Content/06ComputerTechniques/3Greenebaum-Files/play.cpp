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


void play(PCMsound *sound, unsigned char *data)
{
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

    WriteAudioStream(outStream, data, sound->bytes / (sound->bytesPerSample*sound->samplesPerFrame)); 

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

        unsigned char *data;

        switch (audio.magic) {

        case 0x464F524D:    // "FORM"   -- AIFF/AIFC Format
            data = (unsigned char *) audio.aiff->ckSound->data;
            break;

        case 0x2E736E64:    // ".snd"   -- NeXT/Sun Format
            data = (unsigned char *) audio.au->data;
            break;

        case 0x52494646:    // "RIFF"   -- WAV Format
            data = (unsigned char *) audio.wav->ckData->data;
            break;

        default:            // Raw Format
            fprintf(stderr, "Unrecognized audio file format: 0x%08x\n", audio.magic);
            exit(1);
        }

		printf("bytes per sample:  %d\n", audio.sound->bytesPerSample);
		printf("samples per frame: %d\n", audio.sound->samplesPerFrame);
		printf("frames per second: %d\n", audio.sound->framesPerSecond);
		printf("bytes of data:     %d\n", audio.sound->bytes);

        // pack the 4 bytes per sample data into the right number of bytes
        for (int i = 0,j = 0; i < audio.sound->bytes; i += audio.sound->bytesPerSample, j++) {
            void *dest = (char *) data + i;
            int sample = *((int  *) data + j);
            if (audio.sound->bytesPerSample == 1)
                *((char *) dest) = (char) sample;
            else if (audio.sound->bytesPerSample == 2)
                *((short *) dest) = (short) sample;
            else if (audio.sound->bytesPerSample == 4)
                *((int *) dest) = (int) sample;
        }

        play(audio.sound, data);
    }

    // Done
    return 0;
}
