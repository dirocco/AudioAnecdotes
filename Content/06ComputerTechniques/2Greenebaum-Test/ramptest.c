#define RUNTHRESHOLD 2
typedef enum boolean {FALSE, TRUE} boolean;
typedef unsigned short USHORT;

USHORT add(USHORT value1, USHORT value2)
{
return((USHORT) (value1 + value2));
}

USHORT sub(USHORT value1, USHORT value2)
{
return((USHORT) (value1 - value2));
}


main()
{
   USHORT count = 0;        // length of the current run
   int result;              // used to test result of scanf

   USHORT received     = 0; // stores value received from input stream
   USHORT lastReceived = 0; // stores previous value received
   USHORT expected     = 0; // value expected to be seen in input stream

   unsigned int silentCount        = 0;
   unsigned int runCount           = 0;
   unsigned int sampleCount        = -1; // so the first sample will be 0

   // err tracking
   unsigned int lastRunSample      = 0;
   unsigned int lastRunValue       = 0;
   unsigned int zeroCount          = 0;

   // statistics
   unsigned int discontinuityCount = 0;
   unsigned int droppedInstances   = 0;
   unsigned int wrongInstances     = 0;
   unsigned int insertedInstances  = 0;
   unsigned int indeterminateInstances = 0;
   USHORT firstSample;
   unsigned int firstValue;

   int    SignalCount = 0;  


   typedef enum {
      START,             // start state
      EATSILENCE,        // state for waiting for signal (nonzero value)
      RECEIVEDSIGNAL,    // state that indicates a signal is detected
      ESTABLISHRUN, 
      RUNESTABLISHED, 
      REESTABLISHRUN,
      EOF
   } states ;

   states state = START;

   boolean done = FALSE;
   while(!done) {
      lastReceived = received;
      result = scanf("  %d", &received);
      if(result == 1)
	 sampleCount++;
      else
	 state = EOF;

      switch(state) {
	 case START: // first value should be 0 (silence or beggining of ramp)
	    if(received!=0) {
	       printf("signal didn't begin with 0 as expected (rather we received %d at sample 0)\n",
		     received);
	       state = ESTABLISHRUN;
	    } else {
	       state = EATSILENCE;
	       silentCount = 1;
	    }
	 break;

	 case EATSILENCE: // wait for a nonzero value
	    runCount = 1;
	    if(received==1) {
	       silentCount--; // one of the zeros was actually signal
	       printf("signal found at sample %d (after %d silent samples)\n", 
		     sampleCount-1, silentCount);
	       state = ESTABLISHRUN;
	       runCount = 2; // we had at least one zero to get to this state
	    } else if(received) {
	       printf("signal found at sample %d (after %d silent samples)\n", 
		     sampleCount, silentCount);
	       state = ESTABLISHRUN;
	    } else
	       silentCount++;
	 break;

	 case ESTABLISHRUN: // wait for incrementing values
            expected = add(lastReceived, 1);

	    if(received != expected)
	       runCount = 1; // reset count (any sample is a run of 1)
	    else if (++runCount > RUNTHRESHOLD) {
	       state = RUNESTABLISHED;
	       firstSample = sampleCount - runCount + 1;
	       firstValue  = add(received, -1 * (runCount-1));
	       printf("  (%d threshold) sample run found beggining at ", runCount);
	       printf("sample %d (value %d)\n", firstSample, firstValue);
	    }
	 break;

	 case RUNESTABLISHED:
            expected = add(lastReceived, 1);

	    if(received == expected)
	       runCount++;
	    else {
	       lastRunValue  = lastReceived;
	       lastRunSample = sampleCount;
	       indeterminateInstances++;
	       state = REESTABLISHRUN;
	       discontinuityCount++;
	       zeroCount = (lastReceived == 0)?1:0;
	       printf("  %d run ended at sample %d (expected %d, received %d)\n",
		     runCount, sampleCount, expected, received);
	       runCount = 1; // reset count (any sample is a run of 1)
	    }
	 break;

	 case REESTABLISHRUN: // wait for run then determine what went wrong
            expected = add(lastReceived, 1);

	    if(received != expected)
	       runCount = 1; // reset count (any sample is a run of 1)
	    else if (++runCount > RUNTHRESHOLD) {
	       unsigned int newRun = sampleCount - runCount + 1;
	       unsigned int anomalyLength = newRun - lastRunSample;
	       USHORT firstRunValue = add(received, -1 * (runCount-1));

	       state = RUNESTABLISHED;
	       printf("  %d sample run found beggining at ", runCount);
	       printf("sample %d (value %d)\n",
		  sampleCount - runCount + 1, firstRunValue);
	       printf("  %d sample anomaly since last run\n", anomalyLength);

	       printf("signal resumed ");
	       if(firstRunValue == add(lastRunValue, anomalyLength+1)) {
		  wrongInstances++;
		  indeterminateInstances--;
		  printf("in temporal sync\n");
	       } else { // loss of temporal sync
		  if(firstRunValue == add(lastRunValue, 1))
		     printf("where it left off (lost temporal sync)\n");
		  else if(anomalyLength==0) {
		     droppedInstances++;
		     indeterminateInstances--;
		     printf("immediately skipping %d samples (lost temporal sync)\n",
			   sub(firstRunValue, lastRunValue+1));
		  } else {
		     insertedInstances++;
		     indeterminateInstances--;
		     printf("in a random place (lost temporal sync)\n");
		  }
	       }
	    }
	 break;

	 case EOF:
	    done = TRUE;
	 break;

	 default:
	    printf("Reached undefined state, %d, exiting\n", state);
	    exit(-1);
	 break;
      }
   }
   printf("Satistics:\n");
   printf("  %d total samples received\n", sampleCount);
   printf("  %d silent samples before signal\n", silentCount);
   printf("  signal begun at sample %d, value %d\n", firstSample, firstValue);
   // printf("  first run %s with zero\n", (1)?"started":"didn't start");// XXX
   printf("  %d discontinuities (", discontinuityCount);
   printf("%d instances of dropped samples, ", droppedInstances);
   printf("%d instances of incorrect samples, ", wrongInstances);
   printf("%d instances of inserted samples, ", insertedInstances);
   printf("%d instances of indeterminite error)\n", indeterminateInstances);
   printf("Ended %s in sync\n",
	 (lastReceived==add(firstValue, sub(sampleCount, firstSample))) ?"":"not");
}
