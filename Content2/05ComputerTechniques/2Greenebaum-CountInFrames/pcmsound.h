#include <stdio.h>

class PCMsound {
  public:
    PCMsound() { printf("this called?\n"); }
    PCMsound(
             unsigned int framesPerSecond,
	     unsigned char bytesPerSample, 
	     unsigned char samplesPerFrame
	    ) :
             _bytes(0),
             _bytesPerSample(bytesPerSample),
	     _samplesPerFrame(samplesPerFrame),
             _framesPerSecond(framesPerSecond) {}

    // conversions
    unsigned long SecondsToFrames(double seconds) 
              { return((unsigned long)(seconds * _framesPerSecond)); }
    unsigned long FramesToBytes(unsigned long frames)
              { return(frames * _samplesPerFrame * _bytesPerSample); }
    unsigned long SecondsToBytes(double seconds)
              { return(FramesToBytes(SecondsToFrames(seconds)) ); }

           double FramesToSeconds(unsigned long frames)
              { return((double)frames/(double)_framesPerSecond); }
    unsigned long BytesToFrames(unsigned long bytes)
              { return(bytes/_bytesPerSample/_samplesPerFrame); }
           double BytesToSeconds(unsigned long bytes)
              { return(FramesToSeconds(BytesToFrames(bytes))); }

    // set buffer size  by various methods
    void SetBytes(unsigned long bytes)   { _bytes = bytes;                   }
    void SetFrames(unsigned long frames) { _bytes = FramesToBytes(frames);   }
    //void SetSeconds(double seconds)      { _bytes = SecondsToBytes(seconds); }
    void SetSeconds(double seconds)      { printf("SecSec\n"); _bytes = SecondsToBytes(seconds); }
    
    // get stream quantization info
    unsigned char GetBytesPerSample()  { return(_bytesPerSample);  }
    unsigned char GetSamplesPerFrame() { return(_samplesPerFrame); }
    unsigned int  GetFramesPerSecond() { return(_framesPerSecond); }

    // get buffer info
    unsigned long GetBytes()   { return(               _bytes);  }
    unsigned long GetFrames()  { return( BytesToFrames(_bytes)); }
    double        GetSeconds() { return(BytesToSeconds(_bytes)); }

    void print() {
      printf("Frames/Sec: %d, bytes/sample: %d, Samples/Frame: %d\n",
	 GetFramesPerSecond(), GetBytesPerSample(), GetSamplesPerFrame());

      printf("bytes: %d, frames: %d seconds: %f\n\n",
	 GetBytes(), GetFrames(), GetSeconds());
   }

  private:
    // describe the PCM stream quantization
    unsigned char _bytesPerSample;     // both quantization and packing
    unsigned char _samplesPerFrame;    // multiplexing (1 mono, 2 stereo)
    unsigned int  _framesPerSecond;    // confusingly refered to as samplerate

    // describe the buffer
    unsigned long _bytes;              // size of buffer in bytes
};
