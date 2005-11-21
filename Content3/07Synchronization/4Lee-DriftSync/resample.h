#ifndef __RESAMPLE_H__
#define __RESAMPLE_H__

#include <math.h>

#define RESAMPLE_WINDOW_SIZE_HALF 16
#define RESAMPLE_WINDOW_SIZE      (2 * RESAMPLE_WINDOW_SIZE_HALF)

float sinc(float x)
{
   return (x == 0.0f) ? 1.0f : sin(M_PI * x) / (M_PI * x);
}

// This function takes a single channel of audio and resamples by the given rate.
// inBuffer        - input audio sample frames
// numInputFrames  - must be >= numOutputFrames * rate + RESAMPLE_WINDOW_SIZE
// outBuffer       - resampled audio sample frames
// numOutputFrames - number of desired output sample frames
// rate            - rate at which to resample (1.0 == no change)
void Resample(float *inBuffer, int numInputFrames, float *outBuffer, int numOutputFrames, float rate)
{
   int i, j;

   // The first and last RESAMPLE_WINDOW_SIZE_HALF frames are used for interpolating.
   inBuffer += RESAMPLE_WINDOW_SIZE_HALF;

   for (i = 0; i < numOutputFrames; i++)
   {
      float x = i * rate;
      int xInt = (int)x;
      float frac = x - xInt;

      // Accumulate output sample frame using neighbouring samples weighted by a sinc.
      outBuffer[i] = 0.0f;
      for (j = -RESAMPLE_WINDOW_SIZE_HALF; j <= RESAMPLE_WINDOW_SIZE_HALF; j++)
      {
         outBuffer[i] += sinc(-frac + j) * inBuffer[xInt + j];
      }
   }
}

#endif // __RESAMPLE_H__
