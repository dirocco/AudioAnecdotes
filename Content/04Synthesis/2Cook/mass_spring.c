/*************************************/
/*  Mass Spring Damper                  		*/
/*  Sampled Ideal Solution              		*/
/*  Compared to Discrete Approximation  	*/
/*  Perry R. Cook                       		*/
/*  for Audio Anecdotes                 		*/
/************************************/

#include <stdio.h>
#include <math.h>
#include "pablio.h"

#define SAMPLE_RATE 22050.0
#define NUM_FRAMES  1000
//  Play with OVERSAMP parameter to trade accuracy for efficiency
#define OVERSAMP 20.0


//  Mass, Spring, Damping Constants
#define M 0.001
#define K 1000
#define R 0.002

int main(void) {
   long i,j;
   double temp, t, T;             //  Time and (over) sampling period
   //  Buffer and coefficients for digital filter
   double Y[3] = {0,0,0}, coeff1, coeff2; 	
   short output;
   PABLIO_Stream *outStream;
   short buffer[NUM_FRAMES];
   int bufferIndex=0;
   OpenAudioStream(&outStream, SAMPLE_RATE, paInt16, PABLIO_WRITE|PABLIO_MONO);

//  First compute mass/spring/damper from ideal solution        
   for (i=0;i<3*SAMPLE_RATE;i++)   {           	//  Compute 3 seconds
      t = (float) i / (float) SAMPLE_RATE;  		//      of the ideal solution
      output = 20000 * exp(-t) * cos(1000.0 * t);
      buffer[bufferIndex++] = output;
      if (bufferIndex >= NUM_FRAMES){
      	WriteAudioStream(outStream, buffer, NUM_FRAMES);
	bufferIndex = 0;
      }
   }

//  Then compute it from digital approximation
   T = 1.0 / (SAMPLE_RATE * OVERSAMP);       	//  Oversampled rate
   temp =  (M + (T*R) + (T*T*K));      	//  Coefficient denominator
   coeff1 = ((2.0*M) + (T*R)) / temp;
   coeff2 = - M / temp;
   Y[2] = 1000 / OVERSAMP;             	// Normalized initial condition
   for (i=0;i<3*SAMPLE_RATE;i++)   {           	// Compute 3 seconds
      for (j=0;j<OVERSAMP;j++)      {  	//    of digital approximation
   	 Y[0] = (Y[1] * coeff1) +
      	 (Y[2] * coeff2);
	 Y[2] = Y[1];
	 Y[1] = Y[0];
      }
      output = Y[0];
      if (bufferIndex >= NUM_FRAMES){
      	WriteAudioStream(outStream, buffer, NUM_FRAMES);
	bufferIndex = 0;
      }
 
   }
   if (bufferIndex >= 0)
      WriteAudioStream(outStream, buffer, bufferIndex+1);
   CloseAudioStream(outStream);
   
   return 0;
}
