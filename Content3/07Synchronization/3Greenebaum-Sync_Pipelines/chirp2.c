// Produce a chirp every N seconds sample accurately by emitting 
// silent samples to keep time
//
#include <math.h>
#include "pablio.h"

#define SAMPLE_RATE  44100
// #define PERIOD           1.0
//#define PERIOD           0.01
#define PERIOD           1.0

float *createChirp(int sampleRate, double frequency, 
                   int numPeriods, int *numSamples) 
{
   *numSamples = numPeriods * ((double)sampleRate / frequency);
   float *buffer = (float *)malloc(*numSamples * sizeof(float));
int size = *numSamples * sizeof(float);
   double delta = frequency * 2.0 * 3.1415 / sampleRate;
   double theta = 0;
   int x;

   printf("numSamples:%d,  size:%d\n", *numSamples, size);


   for(x = 0; x < *numSamples; x++) {
      buffer[x] = (float)sin(theta);
      theta+= delta;
   }
   return(buffer);
}

void writeFPsilence(PABLIO_Stream *outStream, int numSamples)
{
   int x;
   float sample = 0.0; // silence

   for(x= 0; x<numSamples; x++)
      WriteAudioStream(outStream, &sample, 1); 
}


int main(void)
{
   PABLIO_Stream  *outStream;
 
   int chirpBufferSamples;
   float *chirpBuffer = createChirp(SAMPLE_RATE, 440.0, 1, &chirpBufferSamples);
 
   int stuffSamples = PERIOD * SAMPLE_RATE - chirpBufferSamples;

#ifdef REMOVE
   // ensure that the new method works as well as the old before deleting
   float *stuffBuffer = (float *)malloc(stuffSamples * sizeof(float));
   float *ptr = stuffBuffer;
   int x;
   for (x = 0; x < stuffSamples; x++)
      *ptr = 0.0;
#endif

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);
   while(1) {  // synthesize and output samples forever
      WriteAudioStream(outStream, chirpBuffer, chirpBufferSamples); 
      //WriteAudioStream(outStream, stuffBuffer, stuffSamples); 
      writeFPsilence(outStream, stuffSamples);
   }
}
