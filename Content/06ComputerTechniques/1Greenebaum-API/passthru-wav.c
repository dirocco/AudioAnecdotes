// Microsoft libmm waveio based passthru
//

#include <stdio.h>
#include <wtypes.h>
#define NUM_FRAMES 1000

int main()
{
   // MMRESULT result;
   int inIndex = 0, outIndex = 2;

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
   waveInAddBuffer(inStream, &buffer[0], sizeof(WAVEHDR));
   waveInAddBuffer(inStream, &buffer[1], sizeof(WAVEHDR));
   waveInStart(inStream);
    
   buffer[2].dwFlags |= WHDR_DONE;
   buffer[3].dwFlags |= WHDR_DONE;


   while(1) {
      putchar('a');

      if(buffer[inIndex].dwFlags & WHDR_DONE) {
	 printf("I%d ", inIndex);
	 waveInAddBuffer(  inStream, &buffer[inIndex], sizeof(WAVEHDR));
	 //inIndex = (inIndex==0)?1:0;   // next buffer
	 inIndex = (inIndex+1)%4;   // next buffer
      }

      putchar('c');

      if(buffer[outIndex].dwFlags & WHDR_DONE) {
	 printf("O%d ", outIndex);
	 waveOutWrite(outStream, &buffer[outIndex], sizeof(WAVEHDR));
	 // outIndex = (outIndex==2)?3:2; // next buffer
	 outIndex = (outIndex+1)%4;   // next buffer
      }
putchar('\n');
   }

}
