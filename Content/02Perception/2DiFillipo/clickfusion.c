// click fustion
//
// play two clicks close together
// until they fuse into one sound

#include "stdlib.h"
#include "pablio.h"

#define SAMPLE_RATE 44100

float buffer[SAMPLE_RATE];

int main(int argc, char *argv[])
{
    PABLIO_Stream *outStream;
    int i;

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32,
      PABLIO_WRITE|PABLIO_MONO);

    if(argc==2) {
       double delta = atof(argv[1]); 
       long numsamples = (long)(delta * (double)SAMPLE_RATE);

       // output waveform programmaticaly since delta could exceed buffer
       buffer[0] = 1.0;
       while(numsamples--)
	  WriteAudioStream(outStream, buffer, 1);

    } else if(argc==1) {
       int sampleRate = SAMPLE_RATE;

       for (i = 0; i < SAMPLE_RATE; i++)
	   buffer[i] = 1.0;

       // synthesize and output samples
       while (sampleRate > 50) {
	   printf("%5f seconds (%5d samples) between clicks\n",
	       (float) sampleRate/SAMPLE_RATE, sampleRate);

	   WriteAudioStream(outStream, &buffer, sampleRate);

	   sampleRate *= 0.5;

	   sleep(2);
       }
    } else {
       printf("usage %s [time delay in seconds]\n", argv[0]);
    }

    CloseAudioStream(outStream);
}
