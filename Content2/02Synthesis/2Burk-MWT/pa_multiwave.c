/*********************************************************
 * Generate a band limited sawtooth using wave table synthesis.
 *
 * This example program uses PABLIO, a blocking I/O layer on top
 * of PortAudio. PortAudio is a cross-platform audio I/O library.
 * by Phil Burk and Ross Bencina. For more info visit:
 *     http://www.softsynth.com/portaudio/
 *
 * This code is based on the band limited oscillator code in
 * CSyn, an audio synthesis library for 'C'. CSyn is used as the
 * basis for JSyn - a Java audio synthesis API. Visit:
 *     http://www.softsynth.com/jsyn/
 *
 * (C) 2000 Phil Burk, SoftSynth.com
 * philburk@softsynth.com
 *********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pablio.h"

/**************************** Constants ****************/

#define NON_REAL_TIME   (0)
#define DO_SWEEP_FREQ   (1)

#define SAMPLE_RATE     ((float)44100.0)
#define NYQUIST_RATE    (SAMPLE_RATE/(float)2.0)

#define NUM_TABLES      (8)
#define CYCLE_SIZE      (1<<10)

#define MAX_FRAMES      ((int)(10*SAMPLE_RATE))

#if DO_SWEEP_FREQ
#define START_FREQ      (50.0)
#else
/* If the frequency is close to an integer submultiple of the Nyquist,
 * then you can hear beating between the low harmonics and the high
 * harmonics that get folded back into the audio range!
 */
#define START_FREQ      (NYQUIST_RATE/29.98)
#endif

#define END_FREQ        (NYQUIST_RATE)

#if NON_REAL_TIME
#define FREQUENCY_SCALAR   (1.000015f)
#else
#define FREQUENCY_SCALAR   (1.000010f)
#endif

#ifndef M_PI
#define M_PI            (3.14159265)
#endif

/**************************** Type Definitions ****************/
/* Array of tables for wave table synthesis. */
typedef struct MultiTable
{
	int     mt_NumTables;  // number of tables in stack
	int     mt_CycleSize;  // number of samples in a cycle, table has one extra sample for guard point
	float   mt_PhaseScalar;
	float **mt_Tables;     // pointer to array of tables
} MultiTable;

typedef struct TestData {
	MultiTable    pa_mt;
	float         phase;
	float         freq;
} TestData;

/**************************** Prototypes *********************/
float SawBL_PhaseIncToLevel( float phaseInc );
float SawBL_CalculateSawtooth( MultiTable *mt, float phase,
                               float phaseInc, float flevel );
PaError SawBL_Init( struct MultiTable *mt );
PaError SawBL_Term( struct MultiTable *mt );
static PaError SawBL_CreateTables( MultiTable *mt,
                                 int numTables, int cycleSize );
static float NormalizeArray( float *fdata, int numFloats );

/*****************************************************************************
 * Dissect floating point format number as cheap and dirty way to get
 * a rough log base 2. Ignore sign bit as a cheap way to get absolute value.
 *
 * Use frequency to determine which level of table to use.
 * Level increases with decreasing frequency.
 * flevel = -1 - log2(pInc);
          phaseInc     flevel   interpolate between
     N/1   1.0         -1.0     0.0 and table[0] (pure sine)
	 N/2   0.5          0.0     table[0] and table[1]
	 N/4   0.25         1.0     table[1] and table[2]
	 N/8   0.125        2.0     table[2] and table[3]
 * WARNING - this assumes that floats have the standard IEEE format!
 */
#define MANTISSA_MASK        (0x007FFFFF)
#define MANTISSA_INVERSE     (1.0 / ((float)MANTISSA_MASK))
float SawBL_PhaseIncToLevel( float phaseInc )
{
	float *rp, r; /* MUST USE 32 bit float, not double. */
	unsigned int i, mantissa;
	int exp;
	r = phaseInc;
	rp = &r;
	i = *((unsigned int *) rp);
	exp = ((i >> 23) & 0xFF);
	mantissa = i & MANTISSA_MASK;
	return (float) (126 - (exp + (mantissa * MANTISSA_INVERSE)));;
}

/*******************************************************************
 * Calculate sawtooth value by looking up the value in a table
 * based on the frequency. Interpolate in both directions, between
 * samples in the table, and between tables.
 *
 * Phase ranges from -1.0 to +1.0.
 *
 * When the phaseInc maps to the highest level table,
 * (corresponding to the lowest frequency),
 * then we start interpolating between the highest table
 * and the raw sawtooth value (phase).
 *
 * When phaseInc points to highest table:
 *   flevel = NUM_TABLES - 1 = -1 - log2(pInc);
 *   log2(pInc) = - NUM_TABLES
 *   pInc = 2**(-NUM_TABLES)
 */
#define LOWEST_PHASE_INC_INV ((float)(1 << NUM_TABLES))

float SawBL_CalculateSawtooth( MultiTable *mt, float phase,
									 float phaseInc, float flevel )
{
	float *tableBase;
	float findex;
	int   tableIndex, sampleIndex;
	float verticalFraction;
	float val, horizontalFraction;
	float hiSam; /* Use when verticalFraction is 1.0 */
	float loSam; /* Use when verticalFraction is 0.0 */
	float sam1, sam2;

/* Use Phase to determine sampleIndex into table. */
	findex = (mt->mt_PhaseScalar * phase) + mt->mt_PhaseScalar;
	sampleIndex = (int)findex;
	horizontalFraction = findex - sampleIndex;

/* Truncate toward lower integer. */
	tableIndex = ((int)( flevel + 1000.0)) - 1000;
	verticalFraction = (float) (flevel - tableIndex);

	if( tableIndex > (NUM_TABLES-2) )
	{
	/*
	 * Just use top table and mix with arithmetic sawtooth
	 * if below lowest frequency.
	 * Generate new fraction for interpolating between
	 * 0.0 and lowest table frequency.
	 */
		float fraction = (float) fabs( phaseInc ) * LOWEST_PHASE_INC_INV;
		tableBase = mt->mt_Tables[(NUM_TABLES-1)];

	/* Get adjacent samples. Assume guard point present. */
		sam1 = tableBase[sampleIndex];
		sam2 = tableBase[sampleIndex+1];
	/* Interpolate between adjacent samples. */
		loSam = sam1 + (horizontalFraction * (sam2 - sam1));
		
	/* Use arithmetic version for low frequencies. */
	/* fraction is 0.0 at 0 Hz */
		val = phase + (fraction * (loSam - phase));
	}
	else if( tableIndex < 0 )
	{
		if( tableIndex < -1 )  // above Nyquist!
		{
			val = 0.0;
		}
		else
		{
		/* At top of supported range, interpolate between 0.0 and first partial. */
			tableBase = mt->mt_Tables[0]; /* Sine wave table. */

		/* Get adjacent samples. Assume guard point present. */
			sam1 = tableBase[sampleIndex];
			sam2 = tableBase[sampleIndex+1];
		
		/* Interpolate between adjacent samples. */
			hiSam = sam1 + (horizontalFraction * (sam2 - sam1));
		/* loSam = 0.0 */
			val = verticalFraction * hiSam; /* verticalFraction is 0.0 at Nyquist */
		}
	}
	else
	{
/* Interpolate between adjacent levels to prevent harmonics from popping. */
		tableBase = mt->mt_Tables[tableIndex+1];

	/* Get adjacent samples. Assume guard point present. */
		sam1 = tableBase[sampleIndex];
		sam2 = tableBase[sampleIndex+1];
	
	/* Interpolate between adjacent samples. */
		hiSam = sam1 + (horizontalFraction * (sam2 - sam1));
	
	/* Get adjacent samples. Assume guard point present. */
		tableBase = mt->mt_Tables[tableIndex];
		sam1 = tableBase[sampleIndex];
		sam2 = tableBase[sampleIndex+1];

	/* Interpolate between adjacent samples. */
		loSam = sam1 + (horizontalFraction * (sam2 - sam1));
		
		val = loSam + (verticalFraction * (hiSam - loSam));
	}

	return val;
}

/*******************************************************************/
static float NormalizeArray( float *fdata, int numFloats )
{
	float max, val, gain;
	int i;

// determine maximum value.
	max = 0.0f;
	for( i=0; i<numFloats; i++ )
	{
		val = fdata[i];
		if( val < 0.0f ) val = 0.0f - val;
		if( val > max ) max = val;
	}
	if ( max < 0.0000001f ) max = 0.0000001f;
// scale array
	gain = 1.0f / max;
	for( i=0; i<numFloats; i++ ) fdata[i] *= gain;
	return gain;
}

/*******************************************************************
 * Initialise sawtooth wavetables.
 * Table[0] should contain a pure sine wave.
 * Succeeding tables should have increasing numbers of partials.
 */
static PaError SawBL_CreateTables( MultiTable *mt,
									   int numTables, int cycleSize )
{
	int    i, j, k;
	int    tableSize;
	float *table = NULL;
	float *sineTable = NULL;
	int    bytesPerTable;

/* Add guard point for easier interpolation. */
	tableSize = cycleSize + 1;
	sineTable = (float *) malloc( sizeof(float) * tableSize );
	if( sineTable == NULL ) goto error;

	mt->mt_NumTables = numTables;
	mt->mt_CycleSize = cycleSize;
	mt->mt_Tables = (float **) malloc( sizeof(float *) * numTables);
	if( mt->mt_Tables == NULL ) goto error;
	memset( mt->mt_Tables, 0, sizeof(float *) * numTables );

	mt->mt_Tables[0] = sineTable;
	mt->mt_PhaseScalar = (float) (cycleSize * 0.5);

/* Fill initial sine table with values for -M_PI to M_PI. */
	for( j=0; j<tableSize; j++ )
	{
		sineTable[j] = (float) sin( ((((double)j) / (double)cycleSize ) *
			                             M_PI * 2.0) - M_PI );
	}

/* Allocate other tables. */
	bytesPerTable = sizeof(float) * tableSize;
	for( i=1; i<numTables; i++ )
	{
		float *table = (float *) malloc( bytesPerTable );
		if( table == NULL ) goto error;
		memset( table, 0, bytesPerTable );

		mt->mt_Tables[i] = table;
	}

/* Build each table from scratch by adding sine wave partials.
 * Scale partials by raised cosine to eliminate Gibbs Phenomenon.
 */
	for( i=1; i<numTables; i++ )
	{
		int numPartials;
		double kGibbs;
		table = mt->mt_Tables[i];

/* Add together partials for this table. */
		numPartials = 1 << i;
		kGibbs = M_PI / (2 * numPartials);
		for( k=0; k<numPartials; k++ )
		{
			double ampl, cGibbs;
			int sineIndex = 0;
			int partial = k+1;
			cGibbs = cos(k * kGibbs);
 /* Calculate amplitude for Nth partial */
			ampl = cGibbs * cGibbs / partial;

			for( j=0; j<tableSize; j++ )
			{
				table[j] += (float) ampl * sineTable[sineIndex];
				sineIndex += partial;
/* Wrap index at end of table. */
				if( sineIndex >= cycleSize )
				{
					sineIndex -= cycleSize;
				}
			}
		}
	}

	for( i=1; i<numTables; i++ )
	{
		NormalizeArray( mt->mt_Tables[i], tableSize );
	}
	return 0;

error:
	if( mt->mt_Tables != NULL )
	{
		SawBL_Term( mt );
	}
	else
	{
		if( sineTable != NULL ) free( sineTable );
	}
	return paInsufficientMemory;
}

/*******************************************************************/
PaError SawBL_Init( MultiTable *mt )
{
	return SawBL_CreateTables( mt, NUM_TABLES, CYCLE_SIZE );
}

/*******************************************************************/
PaError SawBL_Term( MultiTable *mt )
{
	int i;
	for( i=0; i<mt->mt_NumTables; i++ )
	{
		if( mt->mt_Tables[i] != NULL )
		{
			free( mt->mt_Tables[i] );
		}
	}
	free( mt->mt_Tables );
	mt->mt_Tables = NULL;
	return 0;
}
/*******************************************************************/
/*********** End of SawBL Utility **********************************/
/*******************************************************************/

/*******************************************************************
 * Generate next band-limited sawtooth value from TestData.
 */
float GenNextSawtoothBL( TestData *data )
{
	float flevel;
	float phaseInc = data->freq / NYQUIST_RATE;
/* Generate raw phaser as the basis for this oscillator. */
	data->phase += phaseInc;
	if( data->phase > 1.0f ) data->phase -= 2.0f;
	else if( data->phase < -1.0f ) data->phase += 2.0f;

/* Calculate table level then use it for lookup. */
	flevel = SawBL_PhaseIncToLevel( phaseInc );
	return SawBL_CalculateSawtooth( &data->pa_mt, data->phase, phaseInc, flevel );
}

/*******************************************************************/
int WriteSamplesToAudio( TestData *data )
{
    PaError         err = 0;
    PABLIO_Stream  *aOutStream;
	float           samples[2];
	int             numFrames = 0;

/* Open blocking I/O stream. */
	err = OpenAudioStream( &aOutStream, SAMPLE_RATE, paFloat32,
				     (PABLIO_WRITE | PABLIO_STEREO) );
	if( err != paNoError ) return err;

	while( (data->freq < END_FREQ) && (numFrames < MAX_FRAMES) )
	{
		float val = GenNextSawtoothBL( data );

/* Write band-limited and non-band-limited sawteeth to audio output. */
		samples[0] = val;
		samples[1] = data->phase;
		WriteAudioStream( aOutStream, samples, 1 );

	/* Glissando */
#if DO_SWEEP_FREQ
		data->freq *= FREQUENCY_SCALAR;
#endif
		numFrames++;
	}

	CloseAudioStream( aOutStream );
	return err;
}

/*******************************************************************/
int WriteSamplesToFile( TestData *data )
{
	FILE  *fid = NULL;
	short  sval;
	int    numWritten;
	int    result = 0;
	int    numFrames = 0;

/* Open binary file to receive raw shorts. */
	fid = fopen("sawbl.raw", "wb");
	if( fid == NULL ) return -1;

	while( (data->freq < END_FREQ) && (numFrames < MAX_FRAMES) )
	{
		float val = GenNextSawtoothBL( data );
	/* Write band-limited sawtooth to left channel. */
		sval = (short) (val * 32767.0);
		numWritten = fwrite( &sval, 1, sizeof(sval), fid );
		if( numWritten != sizeof(sval) )
		{
			result = -1;
			break;
		}
	
	/* Write raw phaser to right channel. */
		sval = (short) (data->phase * 32767.0);
		numWritten = fwrite( &sval, 1, sizeof(sval), fid );
		if( numWritten != sizeof(sval) )
		{
			result = -1;
			break;
		}

	/* Glissando */
#if DO_SWEEP_FREQ
		data->freq *= FREQUENCY_SCALAR;
#endif
		numFrames++;
	}

	if( fid != NULL ) fclose( fid );
	return result;
}

/*******************************************************************/
int main( void );
int main( void )
{
	TestData DATA;
	int result;

	printf("Generate Band-Limited Sawtooth\n");
	printf("by Phil Burk\n");
	printf("Left channel contains band-limited sawtooth.\n");
	printf("Right channel contains NON-band-limited sawtooth.\n");
	printf("START_FREQ = %g\n", START_FREQ);
	fflush(stdout);

/* Initialize band-limited sawtooth data. */
	result = SawBL_Init( &DATA.pa_mt );
	if( result < 0 ) goto error;
	DATA.phase = 0;
	DATA.freq = START_FREQ;

#if NON_REAL_TIME
	result = WriteSamplesToFile( &DATA );
#else
	result = WriteSamplesToAudio( &DATA );
#endif

	SawBL_Term( &DATA.pa_mt );
	printf("Program complete.\n");
	fflush(stdout);
	return 0;

error:
    fprintf( stderr, "Error number: %d\n", result );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( result ) );
	return 1;
}
