#include <math.h>
#include "pablio.h"
#include "resample.h"

#define SAMPLE_RATE      44100.0
#define INPUT_FREQUENCY    440.0
#define NUM_FRAMES          1024
#define MAX_PLAY_RATE       10.0

void GetInputBuffer(float *inBuffer, int startFrameNum, int numFrames)
{
   // Generate a sine wave for the given interval.
   int i;
   for (i = 0; i < numFrames; i++)
   {
      float theta = (i + startFrameNum) * INPUT_FREQUENCY * 2.0 * M_PI / SAMPLE_RATE;
      inBuffer[i] = sin(theta);
   }
}

int main(int argc, char *argv[])
{
   PABLIO_Stream *outStream;
   int inFrameNum = 0;
   float playRate;
   float inBuffer[(int)(NUM_FRAMES * MAX_PLAY_RATE + RESAMPLE_WINDOW_SIZE)];
   float outBuffer[NUM_FRAMES];

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
      // Get the input samples.
      int numInputFrames = (int)ceilf(NUM_FRAMES * playRate);
      GetInputBuffer(inBuffer, inFrameNum - RESAMPLE_WINDOW_SIZE_HALF, numInputFrames + RESAMPLE_WINDOW_SIZE);

      // Resample it to get the desired playrate.
      Resample(inBuffer, numInputFrames + RESAMPLE_WINDOW_SIZE, outBuffer, NUM_FRAMES, playRate);

      // Output it to the audio device.
      WriteAudioStream(outStream, outBuffer, NUM_FRAMES); 

      inFrameNum += numInputFrames;
   }

   return 0;
}
