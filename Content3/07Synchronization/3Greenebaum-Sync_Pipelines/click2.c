// Produce a click every N seconds sample accurately by emitting 
// silent samples to keep time
//
#include <math.h>
#include "pablio.h"

#define SAMPLE_RATE  44100
// #define PERIOD           1.0
#define PERIOD           0.01

unsigned long sampleCount = 0;

int main(void)
{
   PABLIO_Stream  *outStream;
 
   float clickBuffer = 1.0;
   int stuffSamples = PERIOD * SAMPLE_RATE - 1;

   float *stuffBuffer = (float *)malloc(stuffSamples * sizeof(float));
   float *ptr = stuffBuffer;
   int x;
   for (x = 0; x < stuffSamples; x++)
      *ptr = 0.0;

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);
   while(1) {  // synthesize and output samples forever
      WriteAudioStream(outStream, &clickBuffer, 1); 
      WriteAudioStream(outStream, stuffBuffer, stuffSamples); 
   }
}
