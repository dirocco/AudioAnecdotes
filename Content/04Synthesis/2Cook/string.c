#include <stdio.h>
#include "pablio.h"

#define SAMPLE_RATE 22050.0
#define DMAX 256
#define NUM_FRAMES  1000


int main(void) {
   
   int i, j, k, dlength, bufferIndex=0;
   float scale[8] =                            //  E scale frequencies
      {164.81,184.99,207.65,220.00,
       246.94,277.18,311.12,329.62};
   short wg_l[DMAX], wg_r[DMAX];               // Waveguide delay lines
   short output;
   long temp;
   PABLIO_Stream *outStream;
   short buffer[NUM_FRAMES];

   OpenAudioStream(&outStream, SAMPLE_RATE, paInt16, PABLIO_WRITE|PABLIO_MONO);

   for (j=0;j<8;j++)   {                       // Play up the scale
      dlength = SAMPLE_RATE / scale[j] / 2;         // Setup waveguide length
      for (k=0;k<=dlength/2;k++) {            // Setup pluck
	 temp = 20000 * (long) k * 2;
	 temp /= dlength;
	 wg_l[k] = temp;
	 wg_l[dlength-k] = temp;
	 wg_r[k] = temp;
	 wg_r[dlength-k] = temp;
      }
      for (i=0;i<SAMPLE_RATE;i++)   {        // Play each note for 1 second
	 temp = wg_l[0];                     // stash delay outputs
	 output = wg_r[0];
	 for (k=0;k<dlength-1;k++) {         // Do delay lines
	    wg_l[k] = wg_l[k+1];
	    wg_r[k] = wg_r[k+1];
	 }
	 wg_l[dlength-1] = output*-0.99;     // Do reflections
	 wg_r[dlength-1] = -temp;
	 buffer[bufferIndex++] = output;
	 if (bufferIndex >= NUM_FRAMES){
	    WriteAudioStream(outStream, buffer, NUM_FRAMES);
	    bufferIndex = 0;
	 }
      }
   }

   if (bufferIndex > 0)
      WriteAudioStream(outStream, buffer, bufferIndex+1);
   CloseAudioStream(outStream);
}
