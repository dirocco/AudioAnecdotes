// click fustion
//
// play two clicks close together
// until they fuse into one sound

#include "pablio.h"

#define SAMPLE_RATE 44100

float buffer[SAMPLE_RATE];

int main(void)
{
    PABLIO_Stream *outStream;
    int sampleRate = SAMPLE_RATE;
    int i;

    for (i = 0; i < SAMPLE_RATE; i++)
        buffer[i] = 1.0;

    OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32,
        PABLIO_WRITE|PABLIO_MONO);

    // synthesize and output samples
    while (sampleRate > 0) {
        printf("sample rate %5d (%f seconds)\n",
            sampleRate, (float) sampleRate/SAMPLE_RATE);

        WriteAudioStream(outStream, &buffer, sampleRate);

        sampleRate *= 0.5;

        sleep(2);
    }

    CloseAudioStream(outStream);
}
