#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 

#include <list>
using std::list;
#include "pablio.h"

#define HERTZ 44100

//Prototypes --------------------------------------------------
//functions taken from Fouad's Spatialization Article
void Gain(float *in, int size, float *gain, int nch, float *out);
void Pan1D(float *in, int size, float azim, float range, float *out);

//type definitions --------------------------------------------
typedef short sample;

struct frame
{
 	sample l,r;
	//could be more
};

//quickie helper functions ------------------------------------
inline float	HertzToSec(int hz)		{return hz/(float)HERTZ;}
inline int		SecToHertz(float sec)	{return (int)(sec*HERTZ);} 
inline float	frand()					{return (rand()%4096) / 4096.0f;}


//Objects -----------------------------------------------------
struct SoundInstance;

//Structure that encapsulates a sound file loaded
// into memory for use in our demo
struct RawSound
{
	sample *buff;
	int		len;

	//loads a raw sound file
	bool Load		(const char * filename)
	{
		FILE * file = fopen(filename, "rb");
		if(file)
		{
			bool ret = Process(file);
			fclose(file);

			//if something bad happened.
			if(!ret) Release();
			return ret;
		}
		else
			return false;
	}

	//wrapper around delete[]
	void Release	()
	{
		delete[] buff;
		buff	= NULL;
 		len		= 0;
	}

	//debugging routine - dumps the wave content to stdout
	void Print		()
	{
		for(int i = 0; i < len; i++)
		{
			printf("%d\n", buff[i]);
		}
	}

	SoundInstance *MakeInstance();
private:
	sample UnEndianizeShort(unsigned char val1, unsigned char val2)
	{
		return (val2 << 8) | val1;
	}
	
	//internal function for loading a raw wavefile
	bool Process	(FILE * file)
	{
		//find the length of the file (in bytes)
		fseek(file, 0, SEEK_END);	//Goto the end of the file
		len = ftell(file);		//Get the byte index of our position
		len /= sizeof(sample);		//convert from bytes to shorts
		fseek(file, 0, SEEK_SET);	//reset file pointer.

		//allocate a buffer to store data in
		buff = new sample[len];		//read it in
		for(int i = 0; i < len; i++)
		{
			//stackbuffers are for debugging purposes
			// - if they are not zero, something went wrong.
			unsigned char	stackbuffer1=0, val1,
 				stackbuffer2=0, val2,
				stackbuffer3=0;

			//grab the bytes in order
			fread(&val1, 1, 1, file);//I forgot which error to check for here
			fread(&val2, 1, 1, file);

			//if(error) return false;
			//reshuffle them according to platform
			buff[i] = UnEndianizeShort(val1, val2);
		}
		return true;
	}
};

//Structure that encapsulates an instance in which some
// sound file is playing in our demo; note that a single
// soundfile can be playing multiple times (i.e. multiple
// instances) at any given moment
// A Sound Instance is created from the RawSound whose data
// it plays; i.e. it is created from RawSound::MakeInstance()
struct SoundInstance
{
	RawSound &snd;
	float	playrate;	//some minor pitch-shifted for variation
	float	cursor;	//the play cursor on the sound file
	float	vx, vy;		//the positional data of the sound
	bool	isdone;		//whether or not we are done with the sound

	//grab the next sample from a sound source/instance
	sample	GetSample()
	{
		if(!IsDone())
		{
			sample ret = snd.buff[(int)cursor];
			cursor += playrate;
			if(cursor >= snd.len)
				isdone = true;

			return ret;
		}
		return 0;
	}

	void	SetPlayRate(float p)	{playrate = p;}
	bool	IsDone()				{return isdone;}

	SoundInstance(const SoundInstance &si):
		snd(si.snd), playrate(1),cursor(0), isdone(false){}

private:
	//sole purpose of this is to create an instance to be
	// copied by the copy constructor above
	SoundInstance(RawSound &rs):
		snd(rs), playrate(1),cursor(0), isdone(false){}

	friend struct RawSound;
};

//spawns an instance based on the RawSound file loaded
SoundInstance *RawSound::MakeInstance()
{
	return new SoundInstance(*this);
}

//Takes a single sample and split it into stereo
// based on where it is in virtual space
frame SpatiateSample(sample in, float vx, float vy)
{
	const float SAMPLEMAX = (float) 0x7fff;
	float dist = sqrt(vx*vx + vy*vy);	//how far away the source is
	float azim = asin(vx/dist);			//angle off of the forward vector

	float in2 = in / SAMPLEMAX;
	float intermed[2];

	//see Fouad's Article on Spatialization
	Pan1D(&in2, 1, azim, dist, intermed);

	frame ret;	//convert back from float to short
	ret.l = (sample)(intermed[0] * SAMPLEMAX);
	ret.r = (sample)(intermed[1] * SAMPLEMAX);
	return ret;
}

//-------------------------------------
//takes an arbitrary array of samples and dumps it to stdout
void Print(sample * buf, int len)
{
	for(int i = 0; i < len; i++)
	{
		printf("%d\n", buf[i]);
	}
}

//a list of the sound samples currently playing
typedef list<SoundInstance *> SoundQueue;
SoundQueue q;

//returns the currenty stereo sound sample for this iteration
frame GetThisFrame()
{
	frame ret = {0};
		
	SoundQueue::iterator iter;
	for(iter = q.begin(); iter != q.end(); )
	{
		SoundInstance &si = *(*iter);
		
		sample s	= si.GetSample();
		frame f		= SpatiateSample(s, si.vx, si.vy);
		
		ret.l		+= f.l;
		ret.r		+= f.r;	
		
		if((*iter)->IsDone())
		{
			SoundQueue::iterator temp = iter++;
			delete *temp;	//release the memory
			q.erase(temp);	//remove it from the list
		}
		else
			++iter;
	}
	return ret;
}

//The struct that encapsulates the data that determines when a
// sound fires.
struct statmodel
{
	float nextactivate;	//The next time that we will hear the sound

	float timejitter;	//a random push that makes our period less obvious
	float timeoffset;	//the offset of the first sample
	float timeperiod;	//how often in general the sound gets triggered

	int n;				//how many times this sound has played

	statmodel(float j, float o, float p): timejitter(j), timeoffset(o), timeperiod(p)
	{
		n =0;
		nextactivate = 0;
		calcnextactive();
	}
	
	void calcnextactive()
	{
		float temp = /*n*/timeperiod /*+ timeoffset*/ - timejitter + (frand()*2.0*timejitter);
		if(temp < 0.0 ) temp = 0.0;
		nextactivate += temp;
		//n++;
	}
};

//Check if its time to create another sound, and if so, do the
// parameterization to make it slightly different than before.
void UpdateStats(float currenttime, RawSound &rsnd, statmodel &stat)
{
	//update statistical models
	int temp = SecToHertz(stat.nextactivate);
	if(currenttime >=  temp)
	{
		SoundInstance * adding = rsnd.MakeInstance();

		//place the sound somewhere in a 2d planar box from -100 to 100	
		adding->vx = -100 + rand()%200;
		adding->vy = -100 + rand()%200;	

		//pitchshift to create variations in the sound.
		// the random math is an attempt to try to keep
		// the 'val' numbers clustered around zero
		float val	= frand();	// range of   0 <-> 1
		val			-= 0.5f;	// range of -.5 <-> .5
		val = val * val * val;	// -.125 <-> .125

		//if val is clustered around 0, playrate should be clustered around 1
		adding->playrate = 1.0;// + val;

		//add it to the queue to be played immediately
		q.push_back(adding);

		//schedule the next sound
		stat.calcnextactive();
	}
}

// The fun starts here --------------------------------
int main(int argc, char ** argv)
{
	PABLIO_Stream *pstream;
	unsigned int	frameswritten =0;	//our default timer

	//the queue from which to push and pop sounds	
	RawSound		gullsnd;
	RawSound		wavesnd;

	gullsnd.Load("seagull.raw");
	wavesnd.Load("wave.raw");

	OpenAudioStream( &pstream, HERTZ, paInt16, PABLIO_WRITE | PABLIO_STEREO );

	//---				jitter, initial, period
	statmodel gullmodel(0.25,	0.125,	1.0);
	statmodel wavemodel(0.25,	0.0,	0.125);

	SoundInstance *snd = wavesnd.MakeInstance();

	while(1)//(!done)
	{
 		UpdateStats(frameswritten, gullsnd, gullmodel);
		UpdateStats(frameswritten, wavesnd, wavemodel);

		//write out sound samples
		frame writeout = GetThisFrame();
		WriteAudioStream( pstream, &writeout, 1 );
		frameswritten++;
	}

	//cleanup code that doesn't really get called.
	CloseAudioStream( pstream );
	gullsnd.Release();
	wavesnd.Release();
	return 0;
}

//------------------------------------------------------
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