#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "pablio.h"

#define FRAME_RATE      48000
#define SILENT_SAMPLES     10 
#define SILENT_THRESHOLD    0.001

class sdecayingWave 
{
public:
   sdecayingWave(double frequency, double initialAmplitude, double dampingRate,
   int frameRate);

   void render(int bufferSize, float buffer[]);

private:
   double timeDelta, time, freq, amp, damping;
};


sdecayingWave::sdecayingWave(double frequency, double initialAmplitude,
                           double dampingRate, int frameRate)
{ /* set initial values */
   time = 0.0;
   timeDelta = 1.0/frameRate;
   freq = frequency;
   amp = initialAmplitude;
   damping = dampingRate;
}


void sdecayingWave::render(int bufferSize, float buffer[])
{
   int x;

   for(x = 0; x < bufferSize; x++) {
      buffer[x] = 
	 amp * exp(-1.0 * damping * time) * cos(2.0 * M_PI * time * freq);
      time+=timeDelta;
   }
}

#define MIXBUFFER 1000
int main(int argc, char **argv) {
   int waveIndex, waveCount = 0;
   float buf[MIXBUFFER], accumBuf[MIXBUFFER];
   sdecayingWave *waveList[500];

   double freq;                 // setup waves
   double amplitude;
   double damping;

   while(scanf("%lf %lf %lf", &amplitude, &damping, &freq)==3) {
      printf("%f, %f, %f\n",   amplitude,  damping,  freq);
      waveList[waveCount++] = 
         new sdecayingWave(freq, amplitude, damping, FRAME_RATE); 
   }

   // Setup PABLIO
   PABLIO_Stream  *outStream;
   OpenAudioStream(&outStream, FRAME_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);

   int x;
   double sum;
   int done=0;
   while(!done) {
      for(waveIndex = 0; waveIndex < waveCount; waveIndex++) {
         if(!waveIndex) { // initialize accum buffer
            waveList[waveIndex]->render(MIXBUFFER, accumBuf);
         }
         else { // accumulate
            waveList[waveIndex]->render(MIXBUFFER, buf); 
            for(int x = 0; x < MIXBUFFER; x++) {
               accumBuf[x]+= buf[x]; // XXX Realy should saturate...
            }
         }
	 
	 for(x = 0, sum=0.0; x < SILENT_SAMPLES; x++)
	    sum+=fabs(accumBuf[x]);
	 done = sum < SILENT_THRESHOLD;
      }

      WriteAudioStream(outStream, accumBuf, MIXBUFFER);
   } 
   CloseAudioStream(outStream);
}
