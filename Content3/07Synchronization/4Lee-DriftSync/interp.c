#include <math.h>
#include "pablio.h"

#define SAMPLE_RATE      44100.0
#define INPUT_FREQUENCY    440.0
#define NUM_FRAMES          1024
#define MAX_PLAY_RATE       10.0
#define INTERP_TAPS           16

float inBuffer[(int)(NUM_FRAMES * MAX_PLAY_RATE + 2 * INTERP_TAPS)];
float outBuffer[NUM_FRAMES];

float sinc(float x)
{
   if (x == 0.0f)
   {
      return 1.0f;
   }
   else
   {
      return sin(M_PI * x) / (M_PI * x);
   }
}

void GetInputBuffer(float *inBuffer, int startFrameNum, int numFrames)
{
   int i;
   for (i = 0; i < numFrames; i++)
   {
      float theta = (i + startFrameNum) * INPUT_FREQUENCY * 2.0 * M_PI / SAMPLE_RATE;
      inBuffer[i] = sin(theta);
   }
}

void Resample(float *inBuffer, int numInputFrames, float *outBuffer, int numOutputFrames, float rate)
{
   int i = 0;
   int j = 0;
   inBuffer += INTERP_TAPS;

   for (i = 0; i < numOutputFrames; i++)
   {
      float x = i * rate;
      int xInt = (int)x;
      float frac = x - xInt;

      outBuffer[i] = 0.0f;
      for (j = -INTERP_TAPS; j <= INTERP_TAPS; j++)
      {
         outBuffer[i] += sinc(-frac + j) * inBuffer[xInt + j];
      }
   }
}

int main(int argc, char *argv[])
{
   PABLIO_Stream *outStream;
   int inFrameNum = 0;
   float playRate;

   if (argc != 2)
   {
      printf("Usage: %s <play_rate>\n", argv[0]);
      return 1;
   }

   playRate = atof(argv[1]);
   if (playRate > MAX_PLAY_RATE) playRate = MAX_PLAY_RATE;

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE | PABLIO_MONO);

   while (1)
   {
      int numInputFrames = (int)ceilf(NUM_FRAMES * playRate);
      GetInputBuffer(inBuffer, inFrameNum - INTERP_TAPS, numInputFrames + 2 * INTERP_TAPS);

      Resample(inBuffer, numInputFrames, outBuffer, NUM_FRAMES, playRate);

      WriteAudioStream(outStream, outBuffer, NUM_FRAMES); 

      inFrameNum += numInputFrames;
   }

   return 0;
}
