// sine mask
//
// play the reference tone at 1000 Hz (1st second)
// then add a tone at 1100 Hz 40 dB lower which gets masked (2nd second)
// then add a tone at 1300 Hz 40 dB lower which doesn't (3rd second)
// then remove the masked tone (4th second)

#include <math.h>     // sin()
#include "pablio.h"

#define SAMPLE_RATE  44100
#define FREQUENCY     1000.0
#define FREQUENCY1    1100.0
#define FREQUENCY2    1300.0
#define NUM_FRAMES   SAMPLE_RATE

float buffer[NUM_FRAMES];

int main(void)
{
   PABLIO_Stream  *outStream;
   double theta  = 0.0, delta  = FREQUENCY  * 2.0 * 3.1415 / (double) SAMPLE_RATE;
   double theta1 = 0.0, delta1 = FREQUENCY1 * 2.0 * 3.1415 / (double) SAMPLE_RATE;
   double theta2 = 0.0, delta2 = FREQUENCY2 * 2.0 * 3.1415 / (double) SAMPLE_RATE;
   int x;
   int i = 0;

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);

   while(1) {  // synthesize and output samples forever
      for(x= 0; x<NUM_FRAMES; x++) { // synthesize a buffer full of samples
         if (i == 0)
            buffer[x] = 1.0 * sin(theta); // ugly, I know...
         else if (i == 1)
            buffer[x] = 0.9 * sin(theta) +
                        0.1 * sin(theta1); // ugly, I know...
         else if (i == 2)
            buffer[x] = 0.8 * sin(theta) +
                        0.1 * sin(theta1) +
                        0.1 * sin(theta2); // ugly, I know...
         else
            buffer[x] = 0.9 * sin(theta) +
                        0.1 * sin(theta2); // ugly, I know...
         theta+= delta;
         theta1+= delta1;
         theta2+= delta2;
      }

      // blocking write provides flow control
      WriteAudioStream(outStream, buffer, NUM_FRAMES); 

      i = (i + 1) & 0x3;
   }

   CloseAudioStream(outStream);
}
