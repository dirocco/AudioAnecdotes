#include "pablio.h"

#define SAMPLE_RATE  44100
#define NUM_FRAMES    1

float buffer = 1.0;

long before, after;

int main(void)
{
   PABLIO_Stream  *outStream;

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE|PABLIO_MONO);

   while(1) {  // synthesize and output samples forever
      before = GetAudioStreamWriteable(outStream);
      WriteAudioStream(outStream, &buffer, NUM_FRAMES); 
      after = GetAudioStreamWriteable(outStream);
      printf("%ld, %ld\n", before, after);

      sleep(2);
   }
}
