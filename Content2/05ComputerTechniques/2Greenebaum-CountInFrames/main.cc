#include <stdio.h>
#include <stdlib.h>    // malloc/free
#include "pcmsound.h"

class pablioBuffer: public PCMsound {
  public:
    // constructor, specifying stream and buffer size in seconds
    pablioBuffer(
      unsigned  int framesPerSecond,
      unsigned char bytesPerSample,
      unsigned char samplesPerFrame,
             double seconds) : 
         _buffer(0),PCMsound(framesPerSecond, bytesPerSample, samplesPerFrame) {
	 SetSeconds(seconds);
	 _reallocateBuffer();
      }

    // constructor, specifying stream and buffer size in bytes 
     pablioBuffer(
	  unsigned  int framesPerSecond,
          unsigned char bytesPerSample,
          unsigned char samplesPerFrame,
	  unsigned long frames) : _buffer(0),PCMsound(framesPerSecond, bytesPerSample, samplesPerFrame) {
	SetFrames(frames);
	_reallocateBuffer();
     }

     void SetSeconds(double seconds) {
	PCMsound::SetSeconds(seconds);
	_reallocateBuffer();
     }

     void SetFrames(unsigned long frames) {
	PCMsound::SetFrames(frames);
	_reallocateBuffer();
     }

     void SetBytes(unsigned long bytes) {
	PCMsound::SetFrames(bytes);
	_reallocateBuffer();
     }


     ~pablioBuffer() { if(_buffer) free(_buffer); }

     void *getBuffer() {return(_buffer);}

  private:
     void _reallocateBuffer() {
	if(_buffer) free(_buffer);
	_buffer = (void *)malloc(GetBytes());
     }
     void *_buffer;
};


int main() 
{
   pablioBuffer pBuffer(44100, 2, 1, 10.0);
   pBuffer.print();

   pBuffer.SetBytes(88200);
   pBuffer.print();

   pBuffer.SetFrames(22050);
   pBuffer.print();

   pBuffer.SetSeconds(0.1);
   pBuffer.print();
}
