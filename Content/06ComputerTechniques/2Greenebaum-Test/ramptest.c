#define RUNTHRESHOLD 2
typedef enum boolean {FALSE, TRUE} boolean;
typedef unsigned short USHORT;

USHORT add(USHORT value1, USHORT value2)
{
return((USHORT) (value1 + value2));
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
   unsigned int discontinuityCount = 0;

   // err tracking
   unsigned int lastRunSample      = 0;
   unsigned int lastRunValue       = 0;
   unsigned int zeroCount          = 0;

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
	       printf("signal found at sample %d (after %d silent samples)\n", 
		     sampleCount-1, silentCount-1);
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
	       printf("  %d sample run found beggining at sample %d\n", 
		     runCount, sampleCount - runCount + 1);
	    }
	 break;

	 case RUNESTABLISHED:
            expected = add(lastReceived, 1);

	    if(received == expected)
	       runCount++;
	    else {
	       // lastRunValue  = add(expected, -1);
	       lastRunValue  = lastReceived;
	       lastRunSample = sampleCount;
	       state = REESTABLISHRUN;
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
	       state = RUNESTABLISHED;
	       printf("  %d sample run found beggining at sample %d\n", 
		     runCount, sampleCount - runCount + 1);
	    }
	 break;

	 case EOF:
	    done = TRUE;
	 break;

	 default:
	    printf("I fell and I can't get up\n");
	    exit(-1);
	 break;
      }
   }
   printf("Satistics:\n");
   printf("  %d total samples received\n", sampleCount);
   printf("  %d silent samples before signal\n", silentCount);
   printf("  first run %s with zero\n", (1)?"started":"didn't start");// XXX
   printf("  %d discontinuities\n", discontinuityCount);
}
