/*********************************/
/*   Simple Clarinet Model       */
/*   Perry R. Cook               */
/*   Audio Anecdotes             */
/*********************************/

#include <stdio.h>
#include "pablio.h"

#define SRATE 22050.0
#define DMAX 256
#define RESTREFL 0.8
#define RAMP 100.0

void main(void) {
  int i, j = 0, k, dlength, bufferIndex=0;
  float scale[9] =                        //  E scale frequencies
      {164.81,184.99,207.65,220.00,
       246.94,220.0,207.65,184.99,164.81};
  short wg[DMAX] = {0.0};                 // Waveguide delay line
  float Pm,refl;
  float oneZero = 0.0;
  short output;
  long temp;
  FILE *fileOut = fopen("clarinet.raw","wb");
  short buffer[NUM_FRAMES];
  PABLIO_Stream *outStream;

  OpenAudioStream(&outStream, SRATE, paInt16, PABLIO_WRITE|PABLIO_MONO);

  for (j=0;j<9;j++)   {               // Play up and down the scale
    dlength = SRATE / scale[j] / 2;   // Setup waveguide length
    Pm = 0.0;
    for (i=0;i<SRATE/2;i++)   {       // Play each note for 1/2 second
      if (i < 1000) Pm += RAMP;       // Ramp up breath pressure
      else if (i > 9000) Pm -= (RAMP/2);  //   ramp down at end
      output = (wg[0] + oneZero) * 0.50;  // Bell filter, refl and loss
      oneZero = wg[0];
      refl = (float) (Pm - wg[0]);        // Differential reed pressure
      refl /= 50000.0;                    // Normalized
      refl = refl + RESTREFL;             // Bias reed rest position
      if (refl > 1.0) refl = 1.0;         // Reed closed
      if (refl < -1.0) refl = -1.0;       // Reed wide open
      for (k=0;k<dlength-1;k++) {         // Do delay line
          wg[k] = wg[k+1];
      }
      wg[dlength-1] = (refl * -output)    // Reflected bore wave
          + ((1.0 - refl) * Pm);        // and compliment mouth input 
      output *= 20;
      fwrite(&output,2,1,fileOut);
      buffer[bufferIndex++] = output;
      if (bufferIndex >= NUM_FRAMES){
        WriteAudioStream(outStream, buffer, bufferIndex+1);
	bufferIndex=0;
      }
    }
  }
  if (bufferIndex > 0)
    WriteAudioStream(outStream, buffer, bufferIndex+1);
  CloseAudioStream(outStream);
  fclose(fileOut);
}
