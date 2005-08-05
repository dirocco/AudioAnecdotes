#include "pablio.h"

#define SAMPLE_RATE  8000
#define NUM_FRAMES    180
// #define MU
#ifdef MU
#define FILENAME "count-noisy-mu-8000.raw"
#else
#define FILENAME "count-noisy-8-8000.raw"
#endif

#ifdef MU
/*
** This routine converts from ulaw to 16 bit linear.
**
** Craig Reese: IDA/Supercomputing Research Center
** 29 September 1989
**
** References:
** 1) CCITT Recommendation G.711  (very difficult to follow)
** 2) MIL-STD-188-113,"Interoperability and Performance Standards
**     for Analog-to_Digital Conversion Techniques,"
**     17 February 1987
**
** Input: 8 bit ulaw sample
** Output: signed 16 bit linear sample
*/

int
ulaw2linear(ulawbyte)
unsigned char ulawbyte;
{
  static int exp_lut[8] = {0,132,396,924,1980,4092,8316,16764};
  int sign, exponent, mantissa, sample;

  ulawbyte = ~ulawbyte;
  sign = (ulawbyte & 0x80);
  exponent = (ulawbyte >> 4) & 0x07;
  mantissa = ulawbyte & 0x0F;
  sample = exp_lut[exponent] + (mantissa << (exponent + 3));
  if (sign != 0) sample = -sample;

  return(sample);
}
#endif

int main(int argc, char **argv)
{
   FILE *input;
   PABLIO_Stream  *outStream;
   char charSample;
   double sample;
   int count=16;
   double minavg=256.0,avg=0.0,thresh;
   char outbuffer[NUM_FRAMES];
   char zerobuffer[NUM_FRAMES] = { 0 };
   int got= 1;
   int x;

   if ((input = fopen(FILENAME,"rb"))==NULL){
     printf ("Error opening file");
   }else{
     OpenAudioStream(&outStream,SAMPLE_RATE,paInt8,PABLIO_WRITE|PABLIO_MONO);

      x = 0;
      while(1){
         got = fread(&charSample, 1, 1, input);
         if (got)
         {
            sample = charSample;

	    // process every sample
            avg = (fabs(sample)/256.0) + ((255.0 * avg)/256.0);

#ifdef MU
            outbuffer[x++] = ulaw2linear(charSample) >> 8;
#else
            outbuffer[x++] = charSample;
#endif

            if (x < NUM_FRAMES)
               continue;

	    // this computation is supposed to run once per 22.5ms parcel...
            thresh = minavg + 8;
            if (avg > thresh){
// printf("buff %f %f %f %d\n",avg,minavg,thresh,count);
                printf("speech, average = %3.0f\n",avg);
                WriteAudioStream(outStream,outbuffer,NUM_FRAMES);
		count -=1;
		if (count == 0){
			minavg += 1;
			count = 16;
		}
	    }else{
// printf("zero %f %f %f %d\n",avg,minavg,thresh,count);
                printf("silence--------------\n");
                WriteAudioStream(outStream,zerobuffer,NUM_FRAMES);
		if (avg < minavg){
			minavg = avg;
			count = 16;
                }
             }
            x = 0;
        }
        else
           rewind(input);
     }

     if (avg > thresh)
        WriteAudioStream(outStream,outbuffer,x);
     else
        WriteAudioStream(outStream,zerobuffer,x);

     CloseAudioStream(outStream);

     fclose(input);
  }   
}
