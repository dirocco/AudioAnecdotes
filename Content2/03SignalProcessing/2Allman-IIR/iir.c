/********************************************************
//
//	DSP example function - IIR filter
//*********************************************************/
#include <stdio.h>		/* printf debug info */           
#include <stdlib.h>		/* EXIT_SUCCESS return value */

/* function prototype */
float IIR_filter(float*, float*, float*, short);

int main()
{

float delayline[4]	  =	{0.0, 0.0, 0.0, 0.0}; 			/* x[-1] x[-2] y[-1] y[-2]	*/
float samples[5]  = {0.500, 1.000, -0.200, -1.000, 0.500};	 	/* input samples		*/
float coeff[5]	  = {0.22019470027296, 0.44038940054592, 0.22019470027296,
		       0.30756635979221, 0.18834516088404};	/* filter coefficients	*/
short sections    = 1;                                         	/* number of second order sections */
float output;							/* current output		*/

/*   normally this function would be called in a processing  loop  */      

output = IIR_filter(&samples[0], coeff, delayline, sections);	/* perform filter  */
printf("output 0:\t%1.3f\n", output);				/* print result    */

output = IIR_filter(&samples[1], coeff, delayline, sections);    
printf("output 1:\t%1.3f\n", output);

output = IIR_filter(&samples[2], coeff, delayline, sections);    
printf("output 2:\t%1.3f\n", output);

output = IIR_filter(&samples[3], coeff, delayline, sections);    
printf("output 3:\t%1.3f\n", output);

output = IIR_filter(&samples[4], coeff, delayline, sections);    
printf("output 4:\t%1.3f\n", output);

return EXIT_SUCCESS;
}


/**********************************************************
//
//	Function: IIR_filter
//	Purpose: performs an IIR filter on a new sample, then returns
//	Parameters:	input - current input sample	
//			coeff - filter coefficients [b0,b1,b2,a1,a2,b0,b1....]
//			delayline - delayed input and output samples 
//					[x[-1],x[-2],y[-1],y[-2],x[-1].....]
//			sections - number of 2nd order filters to perform.
//			               normally a filter with a larger order
//			               than two is split into a cascade of 
//			               filters.  To perform a filter operation
//			               on an odd order filter you need to
//				 make b[2] and a[2] equal to zero. 
//	Returns: output - output of filter operation
//	Note:	The value of the input sample is changed during
//	 	this routine to the value of the output
************************************************************/

float IIR_filter(float* input, float *coeff, float *delayline, short sections)
{
static float output;                	/* filter output */
static int i;				/* loop counter for filter order > 2	*/
static unsigned int coeff_offset;	/* coeff offset for filter order > 2	*/
static unsigned int delay_offset;	/* delay offset for filter order > 2	*/
output = 0.0;				/* clear filter output value */

for (i = 0; i < sections; i++)		/* loop for filter order > 2 */
{

coeff_offset = 5*i;		/* calculate new coeff offset for filter order > 2	*/
delay_offset = 4*i;             /* calculate new delay offset for filter order > 2	*/

/*BEGIN DEBUG
	printf("input:\t%1.3f\n",input[0]);
	printf("x[-1]:\t%1.3f\n",delayline[0+delay_offset]);
	printf("x[-2]:\t%1.3f\n",delayline[1+delay_offset]);
	printf("y[-1]:\t%1.3f\n",delayline[2+delay_offset]);
	printf("y[-2]:\t%1.3f\n",delayline[3+delay_offset]);
 	printf("   B0:\t%1.3f\n",coeff[0+coeff_offset]);
	printf("   B1:\t%1.3f\n",coeff[1+coeff_offset]);
	printf("   B2:\t%1.3f\n",coeff[2+coeff_offset]);
	printf("   A1:\t%1.3f\n",coeff[3+coeff_offset]);
 	printf("   A2:\t%1.3f\n",coeff[4+coeff_offset]);
END DEBUG*/

output= 	coeff[0+coeff_offset]*input[0] 				/* B0 * x[0]	*/
+ coeff[1+coeff_offset]*delayline[0+delay_offset]		/* B1 * x[-1]	*/
+ coeff[2+coeff_offset]*delayline[1+delay_offset]		/* B2 * x[-2]	*/ 
- coeff[3+coeff_offset]*delayline[2+delay_offset]		/* -A1 * y[-1]	*/
- coeff[4+coeff_offset]*delayline[3+delay_offset];		/* -A2 * y[-2]	*/

delayline[3+delay_offset] = delayline[2+delay_offset];	/* increment output delay line */
elayline[2+delay_offset] = output;
delayline[1+delay_offset] = delayline[0+delay_offset];	/* increment input delay line */
delayline[0+delay_offset] = input[0];
input[0] = output;				/* store new input in case filter order > 2 */	}
return output;
}
