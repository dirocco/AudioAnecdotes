#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "pablio.h"
#include "resample.h"

#define SAMPLE_RATE        44100.0
#define INPUT_FREQUENCY      440.0
#define NUM_FRAMES             512
#define MIN_RESAMPLE_RATE      0.1
#define MAX_RESAMPLE_RATE     10.0
#define FILENAME           "soundclip.raw"

void CalculateSine(float *buffer, int startFrameNum, int numFrames)
{
   // Generate a sine wave for the given interval.
   int i;
   for (i = 0; i < numFrames; i++)
   {
      float theta = (i + startFrameNum) * INPUT_FREQUENCY * 2.0 * M_PI / SAMPLE_RATE;
      buffer[i] = sin(theta);
   }
}

int numInputSamples;
float *inputBuffer;
void GetInputBuffer(float *buffer, int startFrameNum, int numFrames)
{
   if (startFrameNum >= numInputSamples || (startFrameNum + numFrames) < 0)
   {
      // The range that has been requested is completely out of the inputBuffer range.
      memset(buffer, 0, numFrames * sizeof(float));
   }
   else
   {
      int inputStart = startFrameNum > 0 ? startFrameNum : 0;
      int inputEnd = startFrameNum + numFrames;
      if (inputEnd > numInputSamples) inputEnd = numInputSamples;
      
      if (startFrameNum < 0)
      {
         memset(buffer, 0, -startFrameNum * sizeof(float));
         buffer += -startFrameNum;
      }

      memcpy(buffer, inputBuffer + inputStart, (inputEnd - inputStart) * sizeof(float));
      buffer += inputEnd - inputStart;

      if ((startFrameNum + numFrames) > numInputSamples)
      {
         memset(buffer, 0, (startFrameNum + numFrames - numInputSamples) * sizeof(float));
      }
   }
}

int main(int argc, char *argv[])
{
   PABLIO_Stream *outStream;
   int minDrift, maxDrift, totalDrift;
   float driftTime, adjustedRate;
   int useSine = 0;

   int refFrameCount = 0;
   float refBuffer[NUM_FRAMES];

   int depFrameCount = 0, desiredDepFrameCount, numDepFrames;
   float depBuffer[(int)(NUM_FRAMES * MAX_RESAMPLE_RATE + RESAMPLE_WINDOW_SIZE)];
   float depBufferResampled[NUM_FRAMES];

   int outFrameCount = 0;
   float outBuffer[NUM_FRAMES * 2];

   int i;

   if (argc < 4)
   {
      printf("Usage: %s <min drift> <max drift> <drift time> <use sine>\n", argv[0]);
      printf("  min drift  - lower bound of the drift [milliseconds]\n");
      printf("  max drift  - upper bound of the drift [milliseconds]\n");
      printf("  drift time - amount of time between when drift goes from min drift to max drift [seconds]\n");
      printf("  use sine   - 1 to use a sine wave, 0 to use '%s' (default)\n", FILENAME);
      return 1;
   }

   // Convert min/max drift to samples.
   minDrift = (int)(atof(argv[1]) * SAMPLE_RATE / 1000.0);
   maxDrift = (int)(atof(argv[2]) * SAMPLE_RATE / 1000.0);
   driftTime = atof(argv[3]);
   if (argc >= 5)
   {
      useSine = atoi(argv[4]);
   }

   if (!useSine)
   {
      FILE *inputFd = fopen(FILENAME, "rb");
      if (!inputFd)
      {
         printf("Error opening '%s' for reading.  Falling back to sine wave...\n", FILENAME);
         useSine = 0;
      }
      else
      {
         char *rawBuffer;
         struct stat fileStats;
         int i;

         if (stat(FILENAME, &fileStats) != 0)
         {
            printf("Error - could not get file size for '%s'.  Falling back to sine wave...\n", FILENAME);
            useSine = 0;
         }
         else
         {
            numInputSamples = fileStats.st_size;
            rawBuffer = malloc(numInputSamples);
            inputBuffer = (float *)malloc(numInputSamples * sizeof(float));
            fread(rawBuffer, numInputSamples, 1, inputFd);
            for (i = 0; i < numInputSamples; i++)
            {
               inputBuffer[i] = (float)rawBuffer[i] / 255.0;
            }
         }
      }
   }

   // Sanity check...
   if (minDrift > maxDrift)
   {
      int swap = minDrift;
      minDrift = maxDrift;
      maxDrift = swap;
   }

   totalDrift = maxDrift - minDrift;

   if (minDrift > 0)
   {
      refFrameCount = minDrift;
      depFrameCount = 0;
   }
   else
   {
      refFrameCount = 0;
      depFrameCount = -minDrift;
   }
   
   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE | PABLIO_STEREO);

   while (1)
   {
      // Calculate where we want the dependent audio to end up in this next cycle.
      float progress = (outFrameCount + NUM_FRAMES) / (driftTime * SAMPLE_RATE);
      if (progress > 1) progress = 1.0f;
      desiredDepFrameCount = refFrameCount + NUM_FRAMES - (progress * totalDrift + minDrift);

      // Compute a rate so that the dependent audio ends up where we want it to.
      adjustedRate = (float)(desiredDepFrameCount - depFrameCount) / NUM_FRAMES;
      if (adjustedRate < MIN_RESAMPLE_RATE) adjustedRate = MIN_RESAMPLE_RATE;
      else if (adjustedRate > MAX_RESAMPLE_RATE) adjustedRate = MAX_RESAMPLE_RATE;

      printf("r-d: %+.3f msec\tadjustedRate: %.3f\n", (refFrameCount - depFrameCount) * 1000.0 / SAMPLE_RATE, adjustedRate);
//      printf("   progress: %f desired frame count: %d\n", progress, desiredDepFrameCount);
//      printf("   ref: %d dep: %d\n", refFrameCount, depFrameCount);
         
      // Apply the adjusted rate to the dependent track
      numDepFrames = (int)ceilf(NUM_FRAMES * adjustedRate);
      if (useSine)
      {
         CalculateSine(depBuffer, depFrameCount - RESAMPLE_WINDOW_SIZE_HALF, numDepFrames + RESAMPLE_WINDOW_SIZE);
      }
      else
      {
         GetInputBuffer(depBuffer, depFrameCount - RESAMPLE_WINDOW_SIZE_HALF, numDepFrames + RESAMPLE_WINDOW_SIZE);
      }
      Resample(depBuffer, numDepFrames + RESAMPLE_WINDOW_SIZE, depBufferResampled, NUM_FRAMES, adjustedRate);

      // Multiplex audio for output.
      if (useSine)
      {
         CalculateSine(refBuffer, refFrameCount, NUM_FRAMES);
      }
      else
      {
         GetInputBuffer(refBuffer, refFrameCount, NUM_FRAMES);
      }
      for (i = 0; i < NUM_FRAMES; i++)
      {
         float mix = 0.5 * (refBuffer[i] + depBufferResampled[i]);
//         printf("%d\t%f\n", i+outFrameCount, mix);
         outBuffer[2*i]   = mix;
         outBuffer[2*i+1] = mix;
//         outBuffer[2*i]   = refBuffer[i];
//         outBuffer[2*i+1] = depBufferResampled[i];
      }

      // Output to device.
      WriteAudioStream(outStream, outBuffer, NUM_FRAMES); 

      // Update the current time.
      refFrameCount += NUM_FRAMES;
      depFrameCount += numDepFrames;
      outFrameCount += NUM_FRAMES;
   }

   return 0;
}
