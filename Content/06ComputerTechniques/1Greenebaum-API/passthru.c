// This example program uses PABLIO to pass audio input to output
#include "pablio.h"

#define NUM_FRAMES 1000 // number of frames to buffer at a time
                        // size of buffer controls latency
short buffer[NUM_FRAMES];

int main(void)
{
   PABLIO_Stream *inStream, *outStream;

   OpenAudioStream(&inStream,  22050, paInt16, PABLIO_READ);
   OpenAudioStream(&outStream, 22050, paInt16, PABLIO_WRITE);

   while(1) { // pass audio in to audio out one buffer at a time forever
              // using blocking read and write calls which provide flow control
      ReadAudioStream(  inStream, buffer, NUM_FRAMES);

      // process samples in buffer here as desired

      WriteAudioStream(outStream, buffer, NUM_FRAMES);
   }
}
