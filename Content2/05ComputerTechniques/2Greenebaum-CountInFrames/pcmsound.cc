#include <stdio.h>
#include "pcmsound.h"

// PCMsound::PCMsound() { printf("this called?\n"); }

PCMsound::PCMsound(unsigned int framesPerSecond,
	           unsigned char bytesPerSample, 
	           unsigned char samplesPerFrame) :
             _bytes(0),
             _bytesPerSample(bytesPerSample),
	     _samplesPerFrame(samplesPerFrame),
             _framesPerSecond(framesPerSecond) {}

// conversions
unsigned long 
PCMsound::SecondsToFrames(double seconds) 
   { return((unsigned long)(seconds * _framesPerSecond)); }

unsigned long 
PCMsound::FramesToBytes(unsigned long frames)
   { return(frames * _samplesPerFrame * _bytesPerSample); }

unsigned long 
PCMsound::SecondsToBytes(double seconds)
   { return(FramesToBytes(SecondsToFrames(seconds)) ); }


double 
PCMsound::FramesToSeconds(unsigned long frames)
   { return((double)frames/(double)_framesPerSecond); }

unsigned long 
PCMsound::BytesToFrames(unsigned long bytes)
   { return(bytes/_bytesPerSample/_samplesPerFrame); }

double 
PCMsound::BytesToSeconds(unsigned long bytes)
   { return(FramesToSeconds(BytesToFrames(bytes))); }


// set buffer size  by various methods
void PCMsound::SetBytes(unsigned long bytes)   
   { _bytes = bytes;                   }

void PCMsound::SetFrames(unsigned long frames) 
   { _bytes = FramesToBytes(frames);   }

void PCMsound::SetSeconds(double seconds)      
   { _bytes = SecondsToBytes(seconds); }
    
// get stream quantization info
unsigned char PCMsound::GetBytesPerSample()  { return(_bytesPerSample);  }
unsigned char PCMsound::GetSamplesPerFrame() { return(_samplesPerFrame); }
unsigned int  PCMsound::GetFramesPerSecond() { return(_framesPerSecond); }

// get buffer info
unsigned long PCMsound::GetBytes()   { return(               _bytes);  }
unsigned long PCMsound::GetFrames()  { return( BytesToFrames(_bytes)); }
double        PCMsound::GetSeconds() { return(BytesToSeconds(_bytes)); }

void PCMsound::print() {
      printf("Frames/Sec: %d, bytes/sample: %d, Samples/Frame: %d\n",
	 GetFramesPerSecond(), GetBytesPerSample(), GetSamplesPerFrame());

      printf("bytes: %d, frames: %d seconds: %f\n\n",
	 GetBytes(), GetFrames(), GetSeconds());
   }
