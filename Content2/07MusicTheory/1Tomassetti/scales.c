#include <math.h>     // sin()
#include "pablio.h"

#define SAMPLE_RATE  44100
#define NUM_FRAMES   44100

float buffer[NUM_FRAMES];
char strings[13][3] = { "A","Bb","B","C","C#","D","Eb","E","F","F#","G","G#","A" };
float harmonics[13] = { 1./1., 16./15., 9./8., 6./5., 5./4., 4./3., 45./32., 3./2., 8./5., 5./3., 9./5., 15./8., 2./1. };

int main(int argc, char *argv[])
{
   PABLIO_Stream  *outStream;
   double theta = 0.0;
   double delta = 2.0 * 3.1415 / (double)SAMPLE_RATE;
   double frequency, attenuation = 1.0;
   char scale = '\0';
   int x,y;

   frequency = 440.0; // default frequency
   double power = pow(2., 1./12.); // 12th root of 2
   double pythag = 0.0;
   double cents, oldcents = 0.0;
   switch(argc) {
      case 1: break;
      case 2: if (argv[1][0]=='-') scale = argv[1][1];
              break;
      default:
	      printf("usage: %s [-h(armonic),p(ythagorean)]\n",
		    argv[1]);
	      exit(-1);
   }

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, 
	 PABLIO_WRITE|PABLIO_MONO);

   while(1) {  // synthesize and output samples forever
      for(y= 0; y<26; y++) {
         for(x= 0; x<NUM_FRAMES; x++) { // synthesize a buffer full of samples
            buffer[x] = attenuation * sin(theta);
            theta += frequency * delta;
         }

         // blocking write provides flow control
         WriteAudioStream(outStream, buffer, NUM_FRAMES); 

         cents = 1200. * log(frequency/440.) / log(2.);

         printf("note: %-2s frequency: %7.3f cents: %8.3f step: %7.3f\n",strings[y<13?y:25-y],frequency,cents,cents-oldcents);

         oldcents = cents;

         if (y < 12)
            if (scale == 'h')
               frequency = 440. * harmonics[y+1];
            else if (scale == 'p') {
               pythag = (pythag <= 5 ? pythag + 7 : pythag - 5);
               frequency = 440. * pow(3./2., pythag); // 3/2 to the 7th
               while (frequency > 893.0)
                  frequency /= 2.;
            }
            else
               frequency *= power;
         else if (y > 12 && y < 25)
            if (scale == 'h')
               frequency = 440.0 * harmonics[24-y];
            else if (scale == 'p') {
               pythag = (pythag < 7 ? pythag + 5 : pythag - 7);
               frequency = 440. * pow(3./2., pythag); // 3/2 to the 7th
               while (frequency > 893.0)
                  frequency /= 2.;
            }
            else
               frequency /= power;
      }
   }

   CloseAudioStream(outStream);
}
