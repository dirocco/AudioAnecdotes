// This is just a rough first cut at a program that triggers
// like an oscilloscope and measure the time between events
#include "pablio.h"

#define SAMPLE_RATE 44100
#define NUM_FRAMES 1000 // number of frames to buffer at a time
                        // size of buffer controls latency

int main(void)
{
   float buffer[NUM_FRAMES];
   PABLIO_Stream *inStream, *outStream;
   int x;
   double time = 0.0;
   double lastTime = 0.0;
   double delta = 1.0/SAMPLE_RATE;

   OpenAudioStream(&inStream, SAMPLE_RATE, paFloat32, PABLIO_READ|PABLIO_MONO);

   while(1) {
      ReadAudioStream(inStream, buffer, NUM_FRAMES);
      for(x= 0; x < NUM_FRAMES; x++) {
	 if((buffer[x]>0.20)&&(buffer[x]<0.22)&&(x>0)&&(buffer[x]>buffer[x-1])){
	    printf("%f (%f)\n", time, time-lastTime);
	    lastTime = time;
	 }
	 time+=delta;
      }
   }
}
