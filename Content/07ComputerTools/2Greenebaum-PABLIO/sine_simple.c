#include <math.h>     // sin()
#include "pablio.h"

#define SAMPLE_RATE  44100
#define FREQUENCY      440.0


int main(void)
{
   PABLIO_Stream  *outStream;
   double theta = 0.0, delta = FREQUENCY * 2.0 * 3.1415 / (double)SAMPLE_RATE;
   float buffer;
   int x; 

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);

   while(1) {  // synthesize and output samples forever
      buffer = sin(theta); // ugly, I know...
      WriteAudioStream(outStream, &buffer, 1); 
      theta+= delta;
   }
}
