// sine mask
//
// play the reference tone at 1000 Hz and a 2nd tone at a specified amplitude
// and frequency to experiment with masking

#include <math.h>     // sin()
#include "pablio.h"

#define SAMPLE_RATE  44100
#define FREQUENCY     1000.0
#define FREQUENCY1    1100.0
#define NUM_FRAMES   SAMPLE_RATE

float buffer[NUM_FRAMES];

int main(int argc, char *argv[])
{
   PABLIO_Stream  *outStream;

   double theta  = 0.0;
   double theta1 = 0.0;
   double  delta, delta1;
   double frequency1 = FREQUENCY1;
   double attenuation = 0.1;


   if (argc > 1)
      frequency1 = atof(argv[1]);
   if (argc > 2)
      attenuation = atof(argv[2]);

   delta  = FREQUENCY  * 2.0 * 3.1415 / (double) SAMPLE_RATE;
   delta1 = frequency1 * 2.0 * 3.1415 / (double) SAMPLE_RATE;

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);

   int x;
   while(1) {  // synthesize and output samples forever
      for(x= 0; x<NUM_FRAMES; x++) { // synthesize a buffer full of samples
	 buffer[x] = sin(theta) * 0.9 + sin(theta1) * attenuation;
         theta+= delta;
         theta1+= delta1;
      }

      // blocking write provides flow control
      WriteAudioStream(outStream, buffer, NUM_FRAMES); 
   }

   CloseAudioStream(outStream);
}
