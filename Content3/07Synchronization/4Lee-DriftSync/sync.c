#include <math.h>
#include "pablio.h"
#include "resample.h"

#define SAMPLE_RATE      44100.0
#define INPUT_FREQUENCY    440.0
#define NUM_FRAMES          1024
#define MIN_RESAMPLE_RATE    0.1
#define MAX_RESAMPLE_RATE   10.0

// --- "audio" (reference) timebase -------------------------------------------

static float aFrameCount;
static float aBuffer[NUM_FRAMES];

float GetCurrentATime()
{
   return aFrameCount / SAMPLE_RATE;
}

void GetABuffer(float *buffer, int startFrameNum, int numFrames)
{
   // Generate a sine wave for the given interval.
   int i;
   for (i = 0; i < numFrames; i++)
   {
      float theta = (i + startFrameNum) * INPUT_FREQUENCY * 2.0 * M_PI / SAMPLE_RATE;
      buffer[i] = sin(theta);
   }
}


// --- "video" (dependent) timebase -------------------------------------------

static float vFrameCount;
static float vBuffer[(int)(NUM_FRAMES * MAX_RESAMPLE_RATE + RESAMPLE_WINDOW_SIZE)];
static float vBufferResampled[NUM_FRAMES];
static float driftRate = 1.0;

float GetCurrentVTime()
{
   return vFrameCount * driftRate / SAMPLE_RATE;
}

void GetVBuffer(float *buffer, int startFrameNum, int numFrames)
{
   // Generate a sine wave for the given interval.
   // Simulate the clock drift by generating the sine wave at a offsetted frequency
   int i;
   for (i = 0; i < numFrames; i++)
   {
      float theta = (i + startFrameNum) * driftRate * INPUT_FREQUENCY * 2.0 * M_PI / SAMPLE_RATE;
      buffer[i] = sin(theta);
   }
}


// --- main program -----------------------------------------------------------

int main(int argc, char *argv[])
{
   PABLIO_Stream *outStream;
   float deltaT = 2.0 * NUM_FRAMES / SAMPLE_RATE;
   float At0 = 0;
   float Vt0 = 0;
   float At1, At1deltaT, Vt1, Vt1deltaT;
   float adjustedRate = 1.0f;
   int i, numVFrames;
   float outBuffer[2 * NUM_FRAMES];

   if (argc != 3)
   {
      printf("Usage: %s <video_drift_factor> <catchup interval>\n", argv[0]);
      return 1;
   }

   driftRate = atof(argv[1]);
   deltaT = atof(argv[2]) * NUM_FRAMES / SAMPLE_RATE;

   OpenAudioStream(&outStream, SAMPLE_RATE, paFloat32, PABLIO_WRITE | PABLIO_STEREO);

   while (1)
   {
      // Get A(t1), V(t1)
      At1 = GetCurrentATime();
      Vt1 = GetCurrentVTime();
      
      if (At0 != At1)
      {
         // Estimate A(t1+deltaT), V(t1+deltaT)
         At1deltaT = At1 + deltaT * (At1 - At0) / (NUM_FRAMES / SAMPLE_RATE);
         Vt1deltaT = Vt1 + deltaT * (Vt1 - Vt0) / (NUM_FRAMES / SAMPLE_RATE);
         
         // Compute adjusted play rate
         adjustedRate = (At1deltaT - Vt1) / (Vt1deltaT - Vt1) * adjustedRate;
         if (adjustedRate < MIN_RESAMPLE_RATE) adjustedRate = MIN_RESAMPLE_RATE;
         else if (adjustedRate > MAX_RESAMPLE_RATE) adjustedRate = MAX_RESAMPLE_RATE;

         printf("a-v: %+.3f msec\tadjustedRate: %.3f\n", (At1 - Vt1) * 1000, adjustedRate);
         
         // Apply the adjusted rate to the dependent track
         numVFrames = (int)ceilf(NUM_FRAMES * adjustedRate);
         GetVBuffer(vBuffer, vFrameCount - RESAMPLE_WINDOW_SIZE_HALF, numVFrames + RESAMPLE_WINDOW_SIZE);
         Resample(vBuffer, numVFrames + RESAMPLE_WINDOW_SIZE, vBufferResampled, NUM_FRAMES, adjustedRate);
      }
      else
      {
         adjustedRate = 1.0f;
         numVFrames = NUM_FRAMES;
         GetVBuffer(vBufferResampled, vFrameCount, numVFrames);
      }

      // Multiplex audio for output
      GetABuffer(aBuffer, aFrameCount, NUM_FRAMES);
      for (i = 0; i < NUM_FRAMES; i++)
      {
         outBuffer[2*i]   = aBuffer[i];
         outBuffer[2*i+1] = vBufferResampled[i];
      }

      // Output to device
      WriteAudioStream(outStream, outBuffer, NUM_FRAMES); 

      // update A(t0)
      At0 = At1;
      Vt0 = Vt1;
      aFrameCount += NUM_FRAMES;
      vFrameCount += numVFrames;
   }

   return 0;
}
