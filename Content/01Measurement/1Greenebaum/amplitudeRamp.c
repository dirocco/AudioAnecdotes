// amplitude ramp
//
// play the refference tone and print output
// then play -3 dB, -6dB, -10dB, etc.

#include <math.h>     // sin()
#include "pablio.h"

#define SAMPLE_RATE  44100
#define FREQUENCY      440.0
#define NUM_FRAMES    1000

#define INCTIME		0.5
#define RATIO		0.9

float buffer[NUM_FRAMES];

int main(void)
{
   PABLIO_Stream  *outStream;
   double theta=0.0, delta = FREQUENCY * 2.0 * 3.1415 / (double)SAMPLE_RATE;
   int incFrames = INCTIME * SAMPLE_RATE;
   int x; 
   int count = 0;
   double mult = 1.0;

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);

   while(1) {  // synthesize and output samples forever
      for(x= 0; x<NUM_FRAMES; x++) { // synthesize a buffer full of samples
           buffer[x] = mult * sin(theta); // ugly, I know...
           theta+= delta;
	   if(!(count++ % incFrames))
	      mult*= RATIO;
      }

      // blocking write provides flow control
      WriteAudioStream(outStream, buffer, NUM_FRAMES); 
   }
}
