// side tone
// 
// This example program uses PABLIO to pass audio input to output
// with a significant latency, causing "side tone"

#include "pablio.h"

#define SAMPLE_RATE 22050 // number of frames to buffer at a time
#define NUM_FRAMES   1000 // number of frames to buffer at a time
                          // size of buffer controls latency

short buffer[10 * SAMPLE_RATE];

int main(int argc, char *argv[])
{
   PABLIO_Stream *inStream, *outStream;
   int numFrames = NUM_FRAMES;

   if (argc > 1)
      numFrames = (int) (atof(argv[1]) * SAMPLE_RATE);

   // 10 seconds max
   if (numFrames > 10 * SAMPLE_RATE)
      numFrames = 10 * SAMPLE_RATE;
   printf("frames = %5d (seconds = %f)\n",
      numFrames,(float) numFrames / SAMPLE_RATE);

   OpenAudioStream(&inStream,  SAMPLE_RATE, paInt16, PABLIO_MONO|PABLIO_READ);
   OpenAudioStream(&outStream, SAMPLE_RATE, paInt16, PABLIO_MONO|PABLIO_WRITE);

   while(1) { // pass audio in to audio out one buffer at a time forever
              // using blocking read and write calls which provide flow control
      ReadAudioStream(  inStream, buffer, numFrames);

      // process samples in buffer here as desired

      WriteAudioStream(outStream, buffer, numFrames);
   }

   CloseAudioStream(inStream);
   CloseAudioStream(outStream);
}
