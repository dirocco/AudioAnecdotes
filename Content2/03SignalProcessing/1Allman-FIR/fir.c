/********************************************************
//
//	DSP example function - FIR filter
//
*********************************************************/

#include <stdio.h>		/* printf debug info         */
#include <stdlib.h>		/* EXIT_SUCCESS return value */

float FIR_filter(float*, float*, int);

int main()
{
   float seq[3]	    = {0.5, 1.0,-2.0};
   float samples[3] = {0.0, 0.0, 0.0};
   float coeff[3]   = {0.5, 1.0, 0.5};
   float output	    = 0.0;
   int no_coeff	    = 3;


/* calls to FIR filter NOTE: 2n-1 calls to complete filtering
   normally this function would be called in a processing
   loop
*/      

   samples[0] = seq[0];				   /* load new sample */
   output = FIR_filter(samples, coeff, no_coeff);  /* perform filter  */    
   printf("output 0:\t%1.2f\n", output);	   /* print result    */

   samples[0] = seq[1];
   output = FIR_filter(samples, coeff, no_coeff);    
   printf("output 1:\t%1.2f\n", output);

   samples[0] = seq[2];
   output = FIR_filter(samples, coeff, no_coeff);    
   printf("output 2:\t%1.2f\n", output);

   samples[0] = 0.0;
   output = FIR_filter(samples, coeff, no_coeff);    
   printf("output 3:\t%1.2f\n", output);

   samples[0] = 0.0;
   output = FIR_filter(samples, coeff, no_coeff);    
   printf("output 4:\t%1.2f\n", output);
       
   return EXIT_SUCCESS;
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

float FIR_filter(float* samples, float *coeff, int no_coeff)
{
   static float output;	/* output of the FIR filter  */
   static   int      i;	/* loop counter		     */

   output = 0.0;	/* clear summing variable    */

#ifdef DEBUG
   for(i = 0; i < no_coeff; i++)
      printf("sample: %1.2f\tcoeff: %1.2f\n", samples[i], coeff[i]);
#endif /* DEBUG */

   for (i = no_coeff-1; i >= 0; i--) {
      output += samples[i] * coeff[i];
      if (i < no_coeff-1)
	 samples[i+1] = samples[i];
   }

   return output;
}
