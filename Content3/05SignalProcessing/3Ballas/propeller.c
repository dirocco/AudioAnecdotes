// Propeller airplane classifier

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <cstring>
// Uncomment next #include for use under MacOS with Metrowerks C compiler.
//#include <console.h>

#define WAVE_ARRAY_SIZE			32767 	// 2^15 - 1
#define	FREQUENCY_BINS 			512 	// 2^9

short	WaveArray[WAVE_ARRAY_SIZE];
double	FreqArrayLog[FREQUENCY_BINS];
double	FreqArray[FREQUENCY_BINS];
double	FreqArrayDiff[FREQUENCY_BINS];
double	Spectrum[FREQUENCY_BINS];
double	OldSpectrum[FREQUENCY_BINS];
double	CplxSpectrum[FREQUENCY_BINS * 2];
double  TempFreqArray[FREQUENCY_BINS];
double  BinValues[FREQUENCY_BINS];
double	WeightedBins[FREQUENCY_BINS];

void	add2(double *out, double *in1, double *in2, int length);
void	sub2(double *out, double *in1, double *in2, int length);
void	mul2(double *out, double *in1, double *in2, int length);
double	integ(double *data, int length);
void	four1(double *data, int nn, int isign);

short get_AIFF_sound_data(char *filename, short *buffer, int buffer_size);
// Uncomment next line if this code will be compiled and used on an Intel-based
//  computer.
void reorder_short(short *i); 
void initialize_arrays(void);

int main (int argc, char *argv[])
{
    short    i, j, k;
    short    half_array_size;
    short    bin_value_offset = 1;
    short    length, analylen;
    double   deviation, freqskew, freqkurt;
    double   fsum = 0.0, fcount = 0.0;
    double   mean = 0.0, variance = 0.0, sd = 0.0;
    double   var_squared = 0.0, sd_cubed = 0.0;
    double   kurt_numer = 0.0, kurt_denom = 0.0;
    double   moment2 = 0.0, moment3 = 0.0, moment4 = 0.0;
    double   total_spectral_variability = 0.0;
    double   relative_spectral_variability = 0.0;
    
    // Uncomment next line for use under MacOS with Metrowerks C compiler.
    //argc = ccommand(&argv); // also must #include <console.h> 

    if (argc < 2)
	printf("Usage analize <waveform file>\n");
    
    // Read wave file for processing.
    length = get_AIFF_sound_data(argv[1], WaveArray, WAVE_ARRAY_SIZE);
	
	if (length <= 0) return -1; // error check
		
	// Uncomment the following three lines to print out the wave data 
	//   being read in. 
	// printf("First 50 elements of wave array\n");
	// for (i = 0; i < 49; i++) printf("%d, ", WaveArray[i]);
	// printf("%d\n", WaveArray[i]);

    //  Zero the analysis arrays.
    initialize_arrays();

    // Compute FFT.
    // This analysis is iterative.  The wave samples are extracted for FFT
    //   processing in groups of 512 samples.  The extraction steps in 
    //   increments 256 so that overlapping analysis occurs.  This helps to 
    //   reduce the effects of analyzing samples that have been arbitrarily 
    //   segmented.
    
    // Reset file length to a modulus of the FFT analysis window.
    length = (short)(length / FREQUENCY_BINS) * FREQUENCY_BINS;
    analylen = length - FREQUENCY_BINS; 
    half_array_size = FREQUENCY_BINS / 2;

    for (i = 0; i < analylen; i += half_array_size) {
	k = i;

	// Convert 16 bit wave data to float in CplxSpectrum because the 
	//   FFT routine four1() expects to see complex numbers.
	for (j = 0; j < FREQUENCY_BINS * 2; j += 2, k++) {
	    CplxSpectrum[j] = (double)WaveArray[k] / (double)WAVE_ARRAY_SIZE;
	    CplxSpectrum[j+1] = 0.0;
	}
		
	// Apply FFT
	four1(CplxSpectrum - 1, FREQUENCY_BINS, 1);
       
       // Calculate magnitude of complex numbers returned by four1().
       k = 0;

       for (j = 0; j < FREQUENCY_BINS; j += 2, k++) {
           Spectrum[k]
               = sqrt(pow(CplxSpectrum[j], 2) + pow(CplxSpectrum[j+1], 2));
       }

       // Add result to FreqArray.  We want a running total of the spectral
       //   values.
       add2(FreqArray, FreqArray, Spectrum, half_array_size);
       
       // Calculate the absolute difference between successive spectra.  
       //   This will provide information about how much the spectra is
       //   changing.  This is a short term analysis of the spectral
       //   consistency.

	if (i > 255) {
	    sub2(TempFreqArray, Spectrum, OldSpectrum, half_array_size);
			
	    for (j = 0; j < half_array_size; j++)
	        TempFreqArray[j] = fabs(TempFreqArray[j]);
			
	    add2(FreqArrayDiff, FreqArrayDiff, TempFreqArray, half_array_size);
        }


        memcpy((void *)OldSpectrum, (void *)Spectrum, 
            half_array_size * sizeof(double));
    }

    // Calculate a single number from the array that has been used to tally 
    //   the spectral changes.
    total_spectral_variability = integ(FreqArrayDiff, half_array_size);
    
    // "Normalize" this number for the length of the sample.  This number
    //   will reflect the degree of spectral variation from one "moment" to 
    //   the next.  Here is the long term analysis of the short term 
    //   spectral consistency.
    relative_spectral_variability = total_spectral_variability / length;

    printf("%s:\n", argv[1]);

    // Compute moments of the long term spectrum.  First compute the mean.
    //   This value will be in units of the FFT analysis.

    for (i = 0; i < half_array_size; i++)
        BinValues[i] = i + bin_value_offset;
    
    mul2(WeightedBins, BinValues, FreqArray, half_array_size);
    
    fsum = integ(WeightedBins, half_array_size);
    fcount = integ(FreqArray, half_array_size);
    printf("\tfcount = %lf\n \tfsum = %lf\n", fcount, fsum);
    
    mean = fsum/fcount;
    printf("\tMean = %f\n",mean);
    
    // Using the mean, calculate the 2nd, 3rd, and 4th moments about the mean.
    for (i = 0; i < half_array_size; i++) {
	deviation = i + bin_value_offset - mean;
	moment2 = moment2+pow(deviation, 2)*FreqArray[i];
	moment3 = moment3+pow(deviation, 3)*FreqArray[i];
	moment4 = moment4+pow(deviation, 4)*FreqArray[i];
    }

    // Calculate the variance from the 2nd moment.
    variance = moment2/(fcount-1);
	
    sd = sqrt(variance);
	    
    // Skewness and kurtosis are measures of the tails of the distribution.
    //   Calculate these from the 3rd and 4th moments, respectively. 
    sd_cubed = pow(sd, 3);	
    freqskew = (fcount * moment3) / ((fcount - 1) * (fcount - 2) * sd_cubed);
    
    kurt_numer 
	= (fcount * (fcount + 1) * moment4) - (3 * pow(moment2, 2) * (fcount - 1));
    var_squared = pow(variance, 2);
    kurt_denom = (fcount - 1) * (fcount - 2) * (fcount - 3) * var_squared;
    freqkurt = kurt_numer / kurt_denom;

    // Print out the features. 
    printf("\trelative spectral variability = %lf\n", 
	relative_spectral_variability);
    printf("\tMoment^2_%1.0f = %lf\n", fcount, moment2);
    printf("\tMoment^3_%1.0f = %lf\n", fcount, moment3);
    printf("\tMoment^4_%1.0f = %lf\n", fcount, moment4);
    printf("\tSkewness = %lf\n", freqskew);
    printf("\tKurtosis = %lf\n", freqkurt);
	
    // Do the classification.
    if (relative_spectral_variability < 0.02) printf("Airplane Detected\n");
}

void add2(double *out, double *in1, double *in2, int length)
{
    int i;

    for (i = 0; i < length; i++)
	out[i] = in1[i] + in2[i];
}

void sub2(double *out, double *in1, double *in2, int length)
{
    int i;

    for (i = 0; i < length; i++)
	out[i] = in1[i] - in2[i];
}

void mul2(double *out, double *in1, double *in2, int length)
{
    int i;

    for (i = 0; i < length; i++)
	out[i] = in1[i] * in2[i];
}

double integ(double *data, int length)
{
    float sum = 0;
    int i;
	
    for (i = 0; i < length; i++)
        sum += data[i];
	
    return sum;
}

// FFT from Numerical Recipes in C (pp. 411-412) 
// **** Copyrighted Material ****
#define SWAP(a, b) tempr = (a); (a) = (b); (b) = tempr

void four1(double *data, int nn, int isign)
{
    int n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn << 1;
    j = 1;

    for (i = 1; i < n; i += 2) {
		
	if (j > i) {
	    SWAP(data[j], data[i]);
	    SWAP(data[j+1], data[i+1]);
	}

	m = n >> 1;
		
	while (m >= 2 && j > m) {
	    j -= m;
	    m >>= 1;
	}
		
	j += m;
    }

    mmax = 2;
	
    while (n > mmax) {
	istep = 2 * mmax;
	theta = 6.28318530717959 / (isign * mmax);
	wtemp = sin(0.5 * theta);
	wpr = -2.0 * wtemp * wtemp;
	wpi = sin(theta);
	wr = 1.0;
	wi = 0.0;
		
	for (m = 1; m < mmax; m += 2) {

           for (i = m; i <= n; i += istep) {
               j = i + mmax;
               tempr = wr * data[j] - wi * data[j+1];
		tempi = wr * data[j+1] + wi * data[j];
		data[j] = data[i] - tempr;
		data[j+1] = data[i+1] - tempi;
		data[i] += tempr;
		data[i+1] += tempi;
	    }

	    wr = (wtemp = wr) * wpr - wi * wpi + wr;
	    wi = wi * wpr + wtemp * wpi + wi;
	}
	mmax = istep;
    }
}

// Locate AIFF file's sound data and copy a portion of it into buffer.
// Returns the number of elements of size short read into buffer, which
//  should be >= buffer_size, or -1 when an error is reported.
// Important assumptions about the data that are not verified here: 
//  - sound file is assumed to be monaural
//  - sample format is assumed to be 16 bit two's complement
//  - sample rate is 44.1 kHz
//  - byte ordering is assumed to be bigendian (AIFF standard)
// (Uncomment byte reordering code below to use this routine on an Intel-based
// computer.)
short get_AIFF_sound_data(char *filename, short *buffer, int buffer_size)
{
    FILE *infile = NULL;
    unsigned char raw_header_data[4096]; // assumes header is < 4096 bytes
    unsigned char idArray[4];
    short i, isAIFF = 0;
    long length = 0;
    
    infile = fopen(filename, "rb");
    
    if (infile == NULL) {
		printf("Error opening %s for input\n", filename);
		return -1;
    }
    
    // Read in sound file header.  Value returned by fread() is the
	//  number of elements of char size read; this should be 4096 or less.
    length = fread((void *)raw_header_data, sizeof(char), 4096, infile);

    fclose(infile);
	
    if (length <= 0) {
        printf("File %s did not contain any data\n", filename);
        return -1;
    }
    
    // Verify file is in AIF Format.  (Test val of 4092 prevents loop 
    //   from reading out of bnds)
    for (i = 0; i < 4092; i += 2) { // all AIFF header info is 2-byte aligned 
        idArray[0] = raw_header_data[i];
        idArray[1] = raw_header_data[i+1];
        idArray[2] = raw_header_data[i+2];
        idArray[3] = raw_header_data[i+3];
			
        if (idArray[0] == 'A'
            && idArray[1] == 'I'
            && idArray[2] == 'F'
            && idArray[3] == 'F') {
            i += 4; // increment i past 'AIFF' 
            isAIFF = 1;
            break;
        }
    }
    	
    if (isAIFF == 0)
    {
        printf("Input file %s is not in AIF Format\n", filename);
        return -1;
    }
    
    // Locate 'SSND' chunk.
    for (i; i < 4092; i += 2) {
	idArray[0] = raw_header_data[i];
	idArray[1] = raw_header_data[i+1];
	idArray[2] = raw_header_data[i+2];
	idArray[3] = raw_header_data[i+3];
	    
	if (idArray[0] == 'S'
	    && idArray[1] == 'S'
	    && idArray[2] == 'N'
	    && idArray[3] == 'D') {
	    i += 16;  // increment i to the beginning of the raw sound data
	    break;
	}
    }
	
    if (i >= 4092) { // error check
        printf("Input file %s contains no sound data or header is very large\n", filename);
        return -1;
    }

    // Now reopen infile... and read it into WaveArray using i as file pos...
    infile = fopen(filename, "rb");
    if (infile == NULL) {
        printf("Error opening %s for input\n", filename);
        return -1;
    }

	// and read it into WaveArray using i as file position.
    if (fseek(infile, (long)i, SEEK_SET)) { // fseek should rtn 0 if ok
        printf("Error reading input file %s\n", filename);
        fclose(infile);
        return -1;
    } else {
        length = fread((void *)buffer, sizeof(short), buffer_size, infile);

        if (length <= 0) {
            printf("File %s did not containt any data\n", filename);
            fclose(infile);
            return -1;
        }
        
        // Uncomment next line when compiling classifier on computers using
        //  an Intel processor.
#ifdef _X86_
        for(i = 0; i < length; i++) reorder_short(buffer + i);
#endif
    }
    
    fclose(infile);
    return length;
}


// Change the byte order of a short.  This routine is needed for analyzing AIFF
//  data on Intel machines.
/* Uncomment this routine when compiling this code for use on an Intel-based
    computer.
*/
void reorder_short(short *i) 
{
    char * p1, *p2;
    short n;

    p1 = (char *) i;
    p2 = (char *) &n;
    p2[0] = p1[1];
    p2[1] = p1[0];
    *i = n;
}


//  Zero the analysis arrays.  Some of these arrays could be doubly used, 
//     are separately defined in this example to help illustrate the analysis	
void initialize_arrays(void)
{
    short i;
    
    for (i = 0; i < FREQUENCY_BINS; i++)
    {
	FreqArrayDiff[i] = 0.0;
	FreqArrayLog[i] = 0.0;
	FreqArray[i] = 0.0;
	Spectrum[i] = 0.0;
	OldSpectrum[i] = 0.0;
    }
}
