/****************************/
/*  Simple Mandolin Model   */
/*  Perry R. Cook           */
/*  for Audio Anecdotes     */
/****************************/

#include <stdio.h>
#include <stdlib.h>
#include "pablio.h"

#define SAMPLE_RATE 22050.0
#define DMAX 512
#define NUM_FRAMES 1000

int main(void) {
   int i, j, k, dlength, dlength2;
   float scale[8] = {391.99,440.00,493.88,523.25, //  G scale frequencies
		     587.32,659.25,739.99,783.99}; 
   short wg1[DMAX] = {0};                      // Waveguide delay line
   short wg2[DMAX] = {0};                      // 2nd String delay line
   short oneZero,oneZero2;                     // Low pass loop filters
   float mandoBodyDelays[12] = {0.0};          // 12 pole filter delays
   float mandoBodyCoeffs[12] = {               // Mandolin body filter
	    1.792615, -1.843217, 1.399024, -0.887241,  //  coefficients
	    0.772857, -0.671634, 0.648989, -0.690535,
	    0.866150, -0.906709, 0.664901, -0.258501};
   float fnoise;
   short output, output2;
   short temp,slope;
   PABLIO_Stream *outStream;

   OpenAudioStream(&outStream, SAMPLE_RATE, paInt16, PABLIO_WRITE|PABLIO_MONO);

   for (j=0;j<8;j++)   {                      // Play up the scale
      dlength = 2 * SAMPLE_RATE / scale[j];   // Setup waveguide length
      dlength2 = dlength + 1;                 // Detune 2nd string 
      slope = 20000 / dlength;
      temp =  -10000;

      for (k=0;k<=dlength;k++) {              // Pluck
	 fnoise = (float) rand();             // Noise for flavor
	 fnoise = (fnoise - 1073741824.0) / 1073741824.0;
	 fnoise = 1.0 + fnoise;
	 wg1[k] = (temp * fnoise);
	 wg2[k] = (temp * fnoise);
	 temp += slope;
      }

      for (i=0;i<SAMPLE_RATE/2;i++)   {      // Play each note for 1/2 second
	 output  = wg1[0];
	 output2 = wg2[0];

	 for (k=0;k<dlength-1;k++) {         // Do delay lines
	    wg1[k] = wg1[k+1];
	    wg2[k] = wg2[k+1];
	 }

	 wg2[k] = wg2[k+1];                  // one extra delay
	 wg1[dlength-1]  = (output+oneZero)  *0.4995;  // Do reflections
	 wg2[dlength2-1] = (output2+oneZero2)*0.4995;  // Do reflections
	 oneZero  = output;
	 oneZero2 = output2;
	 output   = (output + output2) * 0.05;

	 for (k=11;k>0;k--)  {
	    output += 
	       mandoBodyDelays[k] * mandoBodyCoeffs[k];
	       mandoBodyDelays[k] = mandoBodyDelays[k-1];
	 }

	 output += mandoBodyDelays[0] * mandoBodyCoeffs[0];
	 mandoBodyDelays[0] = output;

	 WriteAudioStream(outStream, &output, 1);
      }
   }
   CloseAudioStream(outStream);
}
