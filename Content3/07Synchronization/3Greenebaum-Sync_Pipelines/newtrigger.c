#include "pablio.h"

#define NUM_FRAMES     1000
#define SAMPLE_RATE   44100
#define TRIGGER_LEVEL    0.2

int main(void)
{
   float buffer[NUM_FRAMES];
   PABLIO_Stream *inStream, *outStream;
   int x;
   double     time = 0.0;
   double lastTime = 0.0;
   double    delta = 1.0 / SAMPLE_RATE;
   double lastSample;

   OpenAudioStream(&inStream, SAMPLE_RATE, paFloat32, PABLIO_READ|PABLIO_MONO);
   ReadAudioStream(inStream, &lastSample, 1); // initialize last sample

   while(1) {
      ReadAudioStream(inStream, buffer, NUM_FRAMES);
      for(x= 0; x < NUM_FRAMES; x++) {
	 if((lastSample<TRIGGER_LEVEL)&&(buffer[x]>=TRIGGER_LEVEL)) {
	    printf("%f (%f)\n", time, time-lastTime);
	    lastTime = time;
	 }
	 time+=delta;
	 lastSample = buffer[x];
      }
   }
}
