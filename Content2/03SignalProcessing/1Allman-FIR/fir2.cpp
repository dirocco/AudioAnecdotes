/********************************************************
//
//	DSP example function - FIR filter
//
*********************************************************/

#include <stdio.h>		/* printf debug info         */
#include <stdlib.h>		/* EXIT_SUCCESS return value */
#include <deque>
#include <vector>

using std::deque;
using std::vector;

typedef deque<float> 	RingBuffer;
typedef vector<float> 	CoefBuffer;

const int 	BUFFERSIZE = 64;


float 	FIR_filter(float newsample, RingBuffer &samples, CoefBuffer &coeff);
bool 	InitBuffers(RingBuffer &rb, CoefBuffer &cb);

/*-----------------------------------------
Inputs:
	Series of fir coefficients from file
	sample data set from standard in
Outputs:
	filtered samples to stdout
*///---------------------------------------
int main(int argc, char *argv[])
{
   float seq[3]	    = {0.5, 1.0,-2.0};

   RingBuffer samples;
   CoefBuffer coeff;

   if(argc!=2) {
      printf("Usage: %s coef-file < samples\n", argv[0]);
      exit(-1);
   }
 
   FILE * file = fopen(argv[1], "rt");
   if(!file) {
      fprintf(stderr, "failed to open file: %s\n", argv[1]);
      exit(-1);
   }
   
   char string[BUFFERSIZE];
   while(fgets(string, BUFFERSIZE, file))
      coeff.push_back(atof(string));
   fclose(file);

   samples.resize(coeff.size()); // size sample FIFO to filter order
   //printf("size: %d\n", coeff.size());

   //InitRingBuffer(samples);


   float output	    		= 0.0; //variable to store output of FIR in

   //read samples in from stdin
   while(fgets(string, BUFFERSIZE, stdin)) {
	output = FIR_filter(atof(string), samples, coeff);
	printf("%f\n", output);		
   }


   for(int n = coeff.size(); n; n--) { //flush input samples all the way through the filter
	output = FIR_filter(0.0, samples, coeff);
	printf("%f\n", output);
   }
}


/**********************************************************
//
//	Function: FIR_filter
//	Purpose: performs an FIR filter on a new sample, then returns
//	Parameters:	samples - pointer to an array of floats
//			containing data to be filtered including 
//			the current sample at [0] and the longest delayed 
//			sample at [no_coeff]. 
//			coeff - pointer to an array containing time
//			reversed filter coefficients
//			no_coeff - number of coefficients, also the
//			size of the samples delay line
//	Returns: output - output of convolution operation
//	Note:  The input samples will be delayed by one when this
//		routine exits so that another sample can be loaded
//		into the [0] position next time the routine is called
************************************************************/
float FIR_filter(float newsample, RingBuffer &samples, CoefBuffer &coeff)
{
   //shift the register
   samples.pop_back();
   samples.push_front(newsample);

#ifdef DEBUGPRINT
   printf("got %f - ", newsample);
   for(int i = 0; i < samples.size(); i++)
      printf("%f ",samples[i]);
   printf("\n");
#endif

   float output=0.0;

   //start at the end
   int rindex ;
   for(rindex= coeff.size() -1; rindex >=0; rindex--)
      output += samples[rindex] * coeff[rindex];

   return output;
}
