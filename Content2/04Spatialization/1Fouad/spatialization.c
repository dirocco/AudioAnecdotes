// Takes a stream of floating point samples from stdin applies panning function
// then outputs stereo multiplexed signal on stdout

#include <stdio.h>
#include <stdlib.h> // atof()

#define PI 3.14159265
#define PI_2 PI/2.0
#define TWO_PI 2.0*PI
#define TORAD PI/180.0

// Gain applies up to 8 gain values to an input waveform generating
// between 1 and 8 channels of output audio.
void Gain(float *in, int size, float *gain, int nch, float *out)
{	
    static float storeGains[8] = {	0.0, 0.0, 0.0, 0.0, 
						0.0, 0.0, 0.0, 0.0};
    float deltaGains[8];
    float *outptr = out;
    int i,j;

    for (i=0; i<nch; i++)
        deltaGains[i] = (gain[i] - storeGains[i]) / size;

    for (i=0; i<size; i++)
        for (j=0; j<nch; j++) {
            *outptr++ = in[i] * storeGains[j];
		storeGains[j] += deltaGains[j];
	}
}


// Balance pans a source along an axis connecting left and right 
// speakers using a linear gain curve.
void Balance(float *mono, int size, float bal, float *stereo)
{
    static float lGain=0.0, rGain=0.0;
    float lDeltaGain;
    float rDeltaGain;
    float lgTemp;
    int i,j=0;

    // Calculate gain for the left speaker
    lgTemp = (bal + 1.0)/2.0;
    // Calculate delta values for the left gain			
    lDeltaGain = (lgTemp - lGain) / size;
    // Calculate delta values for the right gain
    rDeltaGain = ((1.0 - lgTemp)- rGain) / size;

    // Apply gain values and generate 2 channels of audio
    for (i=0; i<size; i++)
    {
        stereo[j++] = mono[i]*lGain;
        stereo[j++] = mono[i]*rGain;
        lGain += lDeltaGain;
        rGain += rDeltaGain;
    }
}

// ********************************************************************

// Pan1D pans a source along an axis connecting two speakers using a 
// constant intensity gain curve.
void Pan1D(float *in, int size, float azim, float range, float *out)
{
    float gain[2];
    float lazim, razim;
    float distFact = 1.0/range;
    float lspeaker = 135.0 * TORAD;
    float rspeaker = 45.0 * TORAD;

    // Wrap the azimuth angle to the -PI to PI range and reflect 
    // sources in the rear to their mirror positions in the front.
    azim = fabs(azim);
    while (azim > PI)
        azim -= TWO_PI;
    azim = fabs(azim);
    // Limit the azimuth angle to lie within the arc formed between the 
    // listener and the two speakers.
    if (azim > lspeaker)
        azim = lspeaker;
    else if (azim < rspeaker)
        azim = rspeaker;
    // Find the angle between the source and the left and right 
    // speakers and calculate the gain for each speaker.
    lazim = lspeaker - azim;
    if (lazim > PI_2)
        gain[0] = 0.0;
    else
        gain[0] = cos(lazim)*distFact;	
    razim = rspeaker - azim;
    if (razim > PI_2)
        gain[1] = 0.0;
    else
        gain[1] = cos(razim)*distFact;
    // Apply gain values to the input waveform and generate interleaved 
    // audio
    Gain(in, size, gain, 2, out);
}


// ********************************************************************

// Pan2D pans a source to a location on the listener's plane using a 
// constant power gain curve.
#define PI 3.14159265
#define PI_2 PI/2.0
#define TWO_PI 2.0*PI
#define TORAD PI/180.0

void Pan2D(float *in, int size, float azim, float range, float *out)
{
    static float speakers[4] = {	45.0*TORAD, 135.0*TORAD, 
						-45*TORAD, -135.0*TORAD};
    float gains[4];
    float distFact = 1.0/range;
    float theta;
    int i;

    // Wrap azimuth angles to the range -PI to PI
    while (azim > PI)
        azim -= TWO_PI;
    while (azim  < -PI)
        azim += TWO_PI;
    // Calculate gain values for each speaker
    for (i=0; i<4; i++)
    {
        theta = fabs(speakers[i] - azim);
// Make sure that the angle measured between source and speaker	     // is acute.
	  if (theta > PI)
		theta = TWO_PI - theta;
	  // Final gain values are formed as the product of azimuth gains
     	  // and distance attenuation factor 1/D
	  if (theta > PI_2)
	      gains[i] = 0.0;
	  else
	      gains[i] = cos(theta)*distFact;
    }
    // Apply gain values to the input waveform and generate interleaved 
    // audio.
    Gain(in, size, gains, 4, out);
}	


void usage(char *name)
{
   fprintf(stderr, "usage: %s g|b|1|2 [balance] | [azimuth range]\n", name);
   fprintf(stderr, "(g)ain, (b)alance, (1)D pan, (2)D pan (pan requires azimuth and range\n");
   exit(-1);
}


int main(int argc, char *argv[])
{
   float inSample[1], outSample[4];
   enum Mode {BALANCE, PAN1D, GAIN, PAN2D} mode;
   float bal, azim, range; // parameters
   float value[2];

   if(argc < 2) {
      usage(argv[0]); // this calls exit
   }
   else {
      int required=0;
      switch (*argv[1]) {
	 case 'g': required=1; mode=GAIN;    break;
	 case 'b': required=1; mode=BALANCE; break;
	 case '1': required=2; mode=PAN1D;   break;
	 case '2': required=2; mode=PAN2D;   break;
	 default:
	    fprintf(stderr, "unknown argument %s\n", argv[1]);
	    usage(argv[0]);
      }

      // printf("argc=%d, required=%d\n", argc, required);

      if(argc!=(required+2)) {
	 fprintf(stderr, "%s %s requires %d arguments\n", 
	       argv[0], argv[1], required);
	 usage(argv[0]); // this calls exit
      } 

      int i;
      for(i=0; i < required; i++) {
	 value[i]=atof(argv[i+2]);
	 // fprintf(stderr, "got value[%d]=(%s)=%f\n", i,argv[i+2], value[i]);
      }

      switch(mode)  {
	 case BALANCE:fprintf(stderr, "Balance %f\n", value[0]); break;
	 case GAIN:   fprintf(stderr, "GAIN %f\n",    value[0]); break;
	 case PAN1D:  fprintf(stderr, "PAN1D azimuth:%f range:%f\n", 
			    value[0], value[1]); break;
	 case PAN2D:  fprintf(stderr, "PAN2D azimuth:%f range:%f\n", 
			    value[0], value[1]); break;
	 default: break;
      }
		  
      while(scanf("%f\n", inSample)==1) {
	 switch(mode)  {
	    case BALANCE: 
	       Balance(inSample, 1, value[0],          outSample);
	       printf("%f\n%f\n", outSample[0],  outSample[1]); break;
	    case PAN1D:
	       Pan1D(inSample, 1,   value[0], value[1], outSample);
	       printf("%f\n%f\n", outSample[0],  outSample[1]); break;
	    case GAIN:
	       Gain(inSample, 1,    &value[0], 1,       outSample);
	       printf("%f\n", outSample[0]); break;
	    case PAN2D:     
	       Pan2D(inSample, 1,   value[0], value[1], outSample);
	       printf("%f\n%f\n%f\n%f\n", 
		  outSample[0], outSample[1], outSample[2], outSample[3]); break;
	    default: break;
	 }
      }
   }
}
