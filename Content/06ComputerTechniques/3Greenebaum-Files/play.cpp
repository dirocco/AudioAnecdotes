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


void play(PCMsound *sound, void *data)
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

		printf("bytes per sample:  %d\n", audio.sound->bytesPerSample);
		printf("samples per frame: %d\n", audio.sound->samplesPerFrame);
		printf("frames per second: %d\n", audio.sound->framesPerSecond);
		printf("bytes of data:     %d\n", audio.sound->bytes);

        SampleData sample;
        void *data = (void *) new char[audio.sound->bytes];

        // pack the 4 bytes per sample data into the right number of bytes
        for (int i = 0; i < audio.sound->bytes / audio.sound->bytesPerSample; i++) {
            sample.get(bs,&audio);

            if (audio.sound->bytesPerSample == 1)
                *((char *) data + i) = (char) sample.data;
            else if (audio.sound->bytesPerSample == 2)
                *((short *) data + i) = (short) sample.data;
            else if (audio.sound->bytesPerSample == 4)
                *((int *) data + i) = (int) sample.data;
        }

        play(audio.sound, data);

        delete[] (char *) data;
    }

    // Done
    return 0;
}
