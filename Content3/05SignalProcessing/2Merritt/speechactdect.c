#include <math.h>     // sin()
#include "pablio.h"

#define SAMPLE_RATE  8000
#define FREQUENCY     440.0
#define NUM_FRAMES     10
#define FILENAME "voice1-mu.raw"
// #define FILENAME "voice1.raw"

int main(void)
{
   FILE *input;
   PABLIO_Stream  *outStream;
   char charSample;
   double sample;
   int count=16;
   double minavg=0.0,avg=0.0,thresh;
   char outbuffer[NUM_FRAMES];
   int got= 1;
   int x;

   if ((input = fopen(FILENAME,"rb"))==NULL){
     printf ("Error opening file");
   }else{
     OpenAudioStream(&outStream,SAMPLE_RATE,paInt8,PABLIO_WRITE|PABLIO_MONO);

      while(got){
         got = fread(&charSample, 1, 1, input);
         {
       //     printf("%08d %02X\n", x++,(char)charSample);
	    outbuffer[0]= (short)charSample;

//printf("%d",outbuffer[0]); 
//	    WriteAudioStream(outStream, outbuffer, 1);

            sample = charSample;

	    // process every sample
            avg = (fabs(sample)/256.0) + ((255.0 * avg)/256.0);
//printf("sample %f, avg %f\n",sample, avg);
 
// XXX FIX THIS !!!!  See next line...
	    // this computation is supposed to run once per 22.5ms parcel...
            thresh = minavg + 8;
            if (avg > thresh){
//printf("here %f\n",avg);
		outbuffer[0]=(short)sample;
		count -=1;
		if (count == 0){
			minavg += 1;
			count = 16;
		}
	    }else{
// printf("avg %X\n",avg);
		outbuffer[0] = 0;
		if (avg < minavg){
			minavg = avg;
			count = 16;
                }
             }
             WriteAudioStream(outStream,outbuffer,1);
        }
     }
  }   
}
