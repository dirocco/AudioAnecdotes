#include <math.h>     // sin()
#include "pablio.h"

#define SAMPLE_RATE  44100
#define NUM_FRAMES    1000

float buffer[NUM_FRAMES];

int main(int argc, char *argv[])
{
   PABLIO_Stream  *outStream;
   double lTheta=0.0 ,rTheta= 0.0;
   double delta = 2.0 * 3.1415 / (double)SAMPLE_RATE;
   double lFrequency, rFrequency, attenuation = 1.0;
   int x; 

   lFrequency=400.0; // default frequency
   switch(argc) {
      case 2: break;
      case 3: lFrequency = atof(argv[2]); break;
      default:
	      printf("usage: %s beat-freq [carrier-freq] (try %s 10 400)\n",
		    argv[1]);
	      exit(-1);
   }

   rFrequency = lFrequency + atof(argv[1]);

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, 
	 PABLIO_WRITE|PABLIO_STEREO);

   while(1) {  // synthesize and output samples forever
      for(x= 0; x<NUM_FRAMES; x++) { // synthesize a buffer full of samples
         buffer[2*x] = attenuation * sin(lTheta);
         lTheta += lFrequency * delta;
         buffer[2*x+1] = attenuation * sin(rTheta);
         rTheta += rFrequency * delta;
      }

      // blocking write provides flow control
      WriteAudioStream(outStream, buffer, NUM_FRAMES); 
   }

   CloseAudioStream(outStream);
}
