// click fustion
//
// play two clicks close together
// until they fuse into one sound

#include "pablio.h"

#define SAMPLE_RATE 44100

float buffer[SAMPLE_RATE];

int main(int argc, char** argv)
{
    PABLIO_Stream *outStream;
    float delta = 1.0;
    int sampleLength;
    int i;

    if (argc > 1)
        delta = atof(argv[1]);
    if (delta > 1.0) delta = 1.0;
    if (delta < 0.0) delta = 0.0;

    sampleLength = delta * SAMPLE_RATE;

    for (i = 0; i < sampleLength; i++)
        buffer[i] = 1.0;

    OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32,
        PABLIO_WRITE|PABLIO_MONO);

    // synthesize and output samples
    if (delta == 1.0) {
        // default: loop delta from 1 second down to 0
        while (sampleLength > 0) {
            printf("sample rate %5d (%f seconds)\n", sampleLength, delta);

            WriteAudioStream(outStream, &buffer, sampleLength);

            delta *= 0.5;
            sampleLength = delta * SAMPLE_RATE;

            sleep(2);
        }
    }
    else {
        printf("sample rate %5d (%f seconds)\n", sampleLength, delta);

        WriteAudioStream(outStream, &buffer, sampleLength);
    }

    CloseAudioStream(outStream);
}
