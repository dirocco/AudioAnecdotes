
// Multi.cpp : Derived from TMULTI.ASM from GV106
//
#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include "pablio.h"


#define SAMPLE_RATE  4000
#define NUM_FRAMES   1000


//Only Arguments:
//First Argument is the choice of which index to use for the array 0-13
int main(int argc, char* argv[])
{
   unsigned char static WWTAB[14][8]= {						  					   
	  //PIHigh   VolumeInit AddSubPoint PulseWidthRate
		   //Vibrato   Delay     DivExp    LinearVolumeAdjust
	{	0xD5,0x1F,0x6D,0x07,0x80,0x04,0x04,0x09},//WWSND0	
	{	0x0F,0xFF,0x11,0x07,0x80,0x04,0x04,0x01},//WWSND1
	{	0xFB,0xC8,0x7F,0x07,0x80,0x07,0x01,0x0D},//WWSND2
	{	0x02,0x30,0x01,0x07,0x80,0x04,0x00,0x01},//WWSND3
	{	0x05,0x20,0xc0,0x0f,0x80,0x03,0xf0,0x07},//WWSND4
	{	0x7A,0x76,0x17,0x01,0x76,0x70,0xFF,0xC0},//WWSND5
	{	0x7A,0x76,0x03,0x01,0x76,0x51,0xFF,0x1F},//WWSND6
	{	0x7A,0x76,0x03,0x07,0x17,0x03,0xFF,0x01},//WWSND7
	{	0x03,0x4D,0xFF,0x07,0x43,0x05,0x00,0x03},//WWSND8
	{	0x03,0x7C,0xFF,0x07,0x43,0x08,0x00,0x03},//WWSND9
	{	0x06,0x14,0xFF,0x01,0x80,0x05,0x05,0x05},//WWSNDA
	{	0x02,0xBA,0xFF,0x07,0x80,0x01,0xFF,0x00},//WWSNDB
	{	0x06,0x30,0x6d,0x07,0x80,0x04,0x04,0x09},//WWSNDC
	{	0x0a,0x00,0xff,0x07,0x80,0x04,0x06,0x01} //WWSNDD example generator
};

   //VariablePointers ;8 TABLE INITIALIZED VARIABLES
   unsigned char PointerIncrementHigh;	//DS	1
   unsigned char Vibrato;		//DS	1 ;AMOUNT OF VIBRATO OFFSET
   unsigned char VolumeInit;		//DS	1 
   unsigned char Delay;			//DS	1 ;DELAY WINDOW
   unsigned char AddSubPoint;		//DS	1 ;ADD/SUBTRACT POINT
   unsigned char DivExp;		//DS	1 ;EXPONENTIAL VOLUME ADJUST
   unsigned char PulseWidthRate;	//DS	1 ;PULSE WIDTH RATE
   unsigned char LinearVolumeAdjust;	//DS	1 ;LINEAR VOLUME ADJUST
   //; END OF TABLE INITIALIZED VARIABLES

   unsigned char *VariablePointers[8] = {
	&PointerIncrementHigh,	
	&Vibrato,	
	&VolumeInit,
	&Delay,
	&AddSubPoint,
	&DivExp,
	&PulseWidthRate,
	&LinearVolumeAdjust
	};

   unsigned short WidthRamp;	//DS	2
   unsigned short Magnitude;	//DS	2
   unsigned  char temp;		//DS	1
   unsigned  char ClientInput;
   unsigned  char DelayCount;

   unsigned short WavePointer,PointerIncrement;
   int i,lw = 0;
   unsigned char DACOutput;
   unsigned short buffer[NUM_FRAMES];
   PABLIO_Stream *outStream;
   PaError result;

   short DIAGDir;
    
   int NotDone;


   if(argc!=2) {
      printf("%s: number 0-13\n", argv[0]);
      exit(-1);
   }

   //first time initialization
   srand( (unsigned)time( NULL ) );	//for C version

   ClientInput = (unsigned char) atoi (argv[1]);
   for(i =0; i < 8; i++) 
      *VariablePointers[i] = WWTAB[ClientInput][i];

   PointerIncrement = PointerIncrementHigh << 8;
   WavePointer = 0x0101;
   Magnitude = (VolumeInit << 8) + VolumeInit;
   WidthRamp = 0x0707;
   NotDone = 1;
   result = OpenAudioStream(&outStream, SAMPLE_RATE, paInt16, 
      PABLIO_WRITE|PABLIO_MONO);

   while(NotDone) {
      if( (WavePointer >> 8) < AddSubPoint ) {
	 DIAGDir = -1;
	 PointerIncrement -= Vibrato;
      } else { //WavePointer is > AddSubPoint
	 DIAGDir = 1;
	 PointerIncrement += Vibrato;
      }

      temp = Magnitude >>8;
      if(DivExp < 9)
	 temp >>= DivExp;	//power of 2 divide by DivExp
      else {
	 if(DivExp > 0x7f)
	    temp >>=1;	//shift before checking
	 else {
	    temp = 0;
	    //in the original 6502 assembler there are cases where this parameter 
	    //would be a delay loop rather than a divide
	    //values 0x70 and 0x51 would clear out temp after 8 
	    //but there were a lot more iterations
	    //the effect of which would be to lower the sampling rate
	 }
      }
      temp |= LinearVolumeAdjust;
      if(!LinearVolumeAdjust) 
	 LinearVolumeAdjust++;  //never allow a temp of 0 - sound will never end
      Magnitude -= temp;	//magnitude swing value
      if( (Magnitude >> 8) == 0x80) 
	 NotDone = 0;

      //Pulse Width Osc Rate upward ramp running at PulseWidthRate rate
      WidthRamp += PulseWidthRate; 

      //Main Osc upward ramp running at PointerIncrement rate 
      //(PointerIncrement is being incremented/decremented by Vibrato) 
      WavePointer += PointerIncrement;

      if( (WavePointer>> 8) >= (WidthRamp>>8))
	 DACOutput = Magnitude >> 8;
      else {	// WavePointer < WidthRamp
	 DACOutput = Magnitude >> 8;
	 DACOutput ^= 0xff;	//invert it
      }

      buffer[0] = (short)(DACOutput<<8);
      WriteAudioStream(outStream, buffer, 1);
   }

   sleep(2);
   CloseAudioStream(outStream);
}
