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
const char * 	InputFile  = "coefs.txt";


float 	FIR_filter	(float newsample, RingBuffer &samples, CoefBuffer &coeff);
bool 	InitBuffers	(RingBuffer &rb, CoefBuffer &cb);

/*-----------------------------------------
Inputs:
	Series of fir coefficients from file
	sample data set from standard in
Outputs:
	filtered samples to file
	(?)filtered sampples to PABLIO
*///---------------------------------------
int main()
{
   float seq[3]	    = {0.5, 1.0,-2.0};

   RingBuffer samples;
   CoefBuffer coeff;
 
   if(!InitBuffers(samples, coeff)) return !EXIT_SUCCESS;

   float output	    		= 0.0; //variable to store output of FIR in
   char buffer[BUFFERSIZE]  	= {0}; //Holds an ASCII string read in from stdin

   //read samples in from stdin
   while(fgets(buffer, BUFFERSIZE, stdin))
   {
	output = FIR_filter(atof(buffer), samples, coeff);
	printf("%f\n", output);		
   }

   int n = coeff.size();   //delay built up in the system

   //flush samples after we have finished getting our input
   while(n > 0 ) //should execute n-1 times
   {
	output = FIR_filter(0.0, samples, coeff);
	printf("%f\n", output);
	n--;
   }

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
float FIR_filter(float newsample, RingBuffer &samples, CoefBuffer &coeff)
{
	//shift the register
	samples.pop_back();
	samples.push_front(newsample);

	float output;

	//start at the end
	int rindex ;
	for(rindex= coeff.size() -1; rindex >=0; rindex--)
		output += samples[rindex] * coeff[rindex];
	
	return output;
}


void ReadCoefsFromFile(FILE * file, CoefBuffer & cbuf) 
{
	int i;
	char buffer[BUFFERSIZE] = {0};


	while(fgets(buffer, BUFFERSIZE, file))
		cbuf.push_back(atof(buffer));
}



void InitRingBuffer(RingBuffer &rb)
{
	for(int i = 0; i < rb.size(); i++)
		rb[i] = 0.0;
}

bool InitBuffers(RingBuffer &rb, CoefBuffer &cb)
{

   FILE * file = fopen(InputFile, "rt");
   if(!file) return false;
   
   ReadCoefsFromFile(file, cb);
   fclose(file);

   rb.resize(cb.size());

   InitRingBuffer(rb);
   
   return true;
}
