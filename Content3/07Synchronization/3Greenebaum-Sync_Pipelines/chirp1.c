#include <math.h>
#include "pablio.h"

#define SAMPLE_RATE  44100
#define PERIOD           1

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

   for (x = 0; x < *numSamples; x++) 
   {
      float hanningCoef = 0.5 + 0.5 * cos(2 * M_PI * x / *numSamples);
      buffer[x] = (float)sin(theta) * hanningCoef;
      theta += delta;
   }

   return (buffer);
}

long before, after;

int main(void)
{
   PABLIO_Stream  *outStream;
   int bufferSamples;
   float *buffer = createChirp(SAMPLE_RATE, 440.0, PERIOD, &bufferSamples);

   printf("buffernumSamples = %d\n", bufferSamples);

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);

   while (1) 
   {
      // synthesize and output samples forever
      // before = GetAudioStreamWriteable(outStream);
      WriteAudioStream(outStream, buffer, bufferSamples); 
      // after = GetAudioStreamWriteable(outStream);
      // printf("%ld, %ld\n", before, after);

      sleep(1);
   }

   free(buffer);
}
