// FilteredNoise3.cpp : Derived from EXP1 from DAC.ASM from GV109
//
#include "stdlib.h"
#include "stdio.h"
#include "time.h"     // time()
#include "pablio.h"

#define SAMPLE_RATE  5000
#define NUM_FRAMES   1000


int NewPointerIncrement(unsigned short *PointerIncrement,
			unsigned char *SlopeDecrement, 
			int *Direction,
			unsigned char *SlopeTarget)	
			//Get New Pointer Increment s
{
   *PointerIncrement -= *SlopeDecrement;
   if ( *PointerIncrement > 0x0080) {
      //do it
      if( (*Direction) == 1 ) {
	    *Direction = -1;   // goin down
	    *SlopeTarget = (rand() & 0x7e);
	 } else {
	    *Direction = 1;   // goin up
	    *SlopeTarget = (rand() & 0x7e) + 0x80;
	    }
   } else {
      return (0); // we are done
   }
   return(1);
}


int main(int argc, char* argv[])
{
   // algorith stuff
   unsigned short PointerIncrement;
   unsigned short WavePointer;
   unsigned char  SlopeDecrement;
   unsigned short OverFlowShort;
   int Direction;
   unsigned char  SlopeTarget;
   int NotDone;

   // PABLIO stuff
   short buffer[NUM_FRAMES];
   PABLIO_Stream *outStream;

   OpenAudioStream(&outStream, SAMPLE_RATE, paInt16, PABLIO_WRITE|PABLIO_MONO);

   //first time initialization

   // seed random number generator to produce different results each time
   srand((unsigned)time(NULL));  


//Intentions to have the 0x07 mask and SlopeDecrement will be user inputs
//What will the lower limit and upper limit for SlopeDecrement
   SlopeDecrement   =  (rand() & 0x07) + 0x03; // XXX ? rand value between 3 and 0A

   //initialize hi byte of PointerIncrement
   PointerIncrement = ((rand() & 0x07) + SlopeDecrement) << 8;  

   printf("SlopeDecrement = %x  PointerIncrement = %x\n",
           SlopeDecrement, PointerIncrement);

   Direction   = 1;	      //initilized for first NewPointerIncrement
   NotDone     = 1;
   WavePointer = 0x7f00;      //start in the middle
   NewPointerIncrement(&PointerIncrement,&SlopeDecrement,&Direction,&SlopeTarget);
   // synthesize main explosion
   while(NotDone){
	 OverFlowShort = WavePointer;
	 WavePointer   = WavePointer + (Direction * PointerIncrement);
	 if ( (Direction * (WavePointer>>8)) > (SlopeTarget*Direction) || 
	       (Direction * OverFlowShort) > (Direction * WavePointer)) {
	    if(Direction * OverFlowShort > WavePointer * Direction)
	       WavePointer = OverFlowShort;		//fix overflow

	    NotDone = NewPointerIncrement(&PointerIncrement, &SlopeDecrement,
	                                  &Direction, &SlopeTarget);
	 }
      // simulate volume hardware
      buffer[0] = (short)(WavePointer - 0x7FFF);
      WriteAudioStream(outStream, buffer, 1);
   }

   //fix up Wave Pointer so that we ramp gently to 0 of a signed converter
   Direction = (WavePointer > 0x8000) ? -1 : 1;

   while((Direction * WavePointer) < (Direction * 0x8000)) {
      WavePointer += Direction * 0x0030;
      buffer[0]=  (short)(WavePointer - 0x7FFF);
      WriteAudioStream(outStream, buffer, 1);
   }

   CloseAudioStream(outStream);

   return 0;
}
