// side tone
// 
// This example program uses PABLIO to pass audio input to output
// with a variable latency

#include "pablio.h"

#define SAMPLE_RATE 44100
#define NUM_FRAMES     10   // number of frames to buffer at a time

short buffer[2 * SAMPLE_RATE];

int main(int argc, char *argv[])
{
   PABLIO_Stream *inStream, *outStream;
   int numFrames = 0;
   int firstTime = 1;

   if (argc > 1)
      numFrames = (int) (atof(argv[1]) * SAMPLE_RATE);

   printf("frames = %5d (seconds = %f)\n", numFrames,(float) numFrames / SAMPLE_RATE);

   OpenAudioStream(&inStream,  SAMPLE_RATE, paInt16, PABLIO_MONO|PABLIO_READ);
   OpenAudioStream(&outStream, SAMPLE_RATE, paInt16, PABLIO_MONO|PABLIO_WRITE);

   while(1) { // pass audio in to audio out one buffer at a time forever
              // using blocking read and write calls which provide flow control
      ReadAudioStream(  inStream, buffer, NUM_FRAMES);

      if(firstTime) {
         firstTime = 0;
	 buffer[0] = 0;      // 'steal' the first sample
	 while(numFrames--)  //  make output lag input but adding silence
	    WriteAudioStream(outStream, buffer, 1);
      } 
      WriteAudioStream(outStream, buffer, NUM_FRAMES);
   }

   CloseAudioStream(inStream);
   CloseAudioStream(outStream);
}
