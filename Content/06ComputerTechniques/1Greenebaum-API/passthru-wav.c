// Microsoft libmm waveio based passthru
//

#include <stdio.h>
#include <wtypes.h>
#define NUM_FRAMES 1000

int main()
{
   // MMRESULT result;
   int inIndex, outIndex;

   HWAVEIN   inStream;
   HWAVEOUT outStream;
   WAVEFORMATEX waveFormat;
   WAVEHDR buffer[4];                             // pingpong buffers

   waveFormat.wFormatTag      = WAVE_FORMAT_PCM;  // PCM audio
   waveFormat.nSamplesPerSec  =           22050;  // really 22050 frames/sec
   waveFormat.nChannels       =               2;  // stereo
   waveFormat.wBitsPerSample  =              16;  // 16bits per sample
   waveFormat.cbSize          =               0;  // no extra data
   waveFormat.nBlockAlign     =
      waveFormat.nChannels * waveFormat.wBitsPerSample / 2;
   waveFormat.nAvgBytesPerSec =
      waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;

   // Event: default security descriptor, Manual Reset, initial non-signaled
   HANDLE event = CreateEvent(NULL, TRUE, FALSE, "waveout event");

   waveInOpen(  &inStream, WAVE_MAPPER, &waveFormat, (unsigned long)event, 
      0, CALLBACK_EVENT);
   waveOutOpen(&outStream, WAVE_MAPPER, &waveFormat, (unsigned long)event, 
      0, CALLBACK_EVENT);

   // initialize the input and output PingPong buffers
   int index;
   for(index = 0; index < 4; index++) {
      buffer[index].dwBufferLength = NUM_FRAMES * waveFormat.nBlockAlign;
      buffer[index].lpData         = 
	 (void *)malloc(NUM_FRAMES * waveFormat.nBlockAlign);

      buffer[index].dwFlags        = 0;
/*
      if(index<2)
	 waveInPrepareHeader(  inStream, &buffer[index], sizeof(WAVEHDR));
      else
	 waveOutPrepareHeader(outStream, &buffer[index], sizeof(WAVEHDR));
*/
      waveInPrepareHeader(  inStream, &buffer[index], sizeof(WAVEHDR));


      //buffer[index].dwFlags       |= WHDR_DONE;
   }

   ResetEvent(event);
   for(index= 0; index < 4; index++) // queue all buffers for input
      waveInAddBuffer(inStream, &buffer[index], sizeof(WAVEHDR));
   waveInStart(inStream);
    
   // buffer[2].dwFlags |= WHDR_DONE;
   // buffer[3].dwFlags |= WHDR_DONE;

   // collect 2 full inputs before continuing?
   unsigned long count = 0;
   while(!( buffer[0].dwFlags & WHDR_DONE))
      count++;
   printf("I[0] %f\n", count/100000.0);

   while(!( buffer[1].dwFlags & WHDR_DONE))
      count++;
   printf("I[1] %f\n", count/100000.0);

   inIndex = 2; // the next (filling) inBuffer to watch

   // move the two full buffers to output
   waveOutWrite(outStream, &buffer[0], sizeof(WAVEHDR));
   waveOutWrite(outStream, &buffer[1], sizeof(WAVEHDR));
   outIndex = 0; // the next (draining) outbuffer to watch


   while(1) {
      if(buffer[inIndex].dwFlags & WHDR_DONE) {
	 printf("I[%d] %f\n", inIndex, count/100000.0);
	 waveInAddBuffer(  inStream, &buffer[inIndex], sizeof(WAVEHDR));
	 inIndex = (inIndex+1)%4;   // next buffer
      }

      if(buffer[outIndex].dwFlags & WHDR_DONE) {
	 printf("O[%d] %f\n", outIndex, count/100000.0);
	 waveOutWrite(outStream, &buffer[outIndex], sizeof(WAVEHDR));
	 outIndex = (outIndex+1)%4;   // next buffer
      }
      
      count++;
   }

}
