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
   USHORT LastReceived = 0; // stores previous value received
   USHORT expected     = 0; // value expected to be seen in input stream

   unsigned int silentCount        = 0;
   unsigned int runCount           = 0;
   unsigned int sampleCount        = 0;
   unsigned int discontinuityCount = 0;

   int    SignalCount = 0;  
   int    Total = 0;        // total number of input values read
   int    ErrorCount = 0;   // total number of errors detected
   int    ErrorCountRepeatedValue = 0;
   int    ErrorCountSkippedValue  = 0;
   int    ErrorCountWrongValue    = 0;


   typedef enum {
      START,             // start state
      EATSILENCE,        // state for waiting for signal (nonzero value)
      RECEIVEDSIGNAL,    // state that indicates a signal is detected
      ESTABLISHRUN, 
      RUNESTABLISHED, 
      REESTABLISHRUN,
      ERROR,
      EOF
   } states ;

   states state = START;

   boolean done = FALSE;
   while(!done) {
      LastReceived = received;
      result = scanf("%d", &received);
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
		     sampleCount-2, silentCount-1);
	       state = ESTABLISHRUN;
	       runCount = 2; // we had at least one zero to get to this state
	    } else if(received) {
	       printf("signal found at sample %d (after %d silent samples)\n", 
		     sampleCount-1, silentCount);
	       state = ESTABLISHRUN;
	    } else
	       silentCount++;
	 break;

	 case ESTABLISHRUN: // wait for incrementing values
            expected = add(LastReceived, 1);

	    if(received != expected)
	       runCount = 1; // reset count (any sample is a run of 1)
	    else if (++runCount > RUNTHRESHOLD) {
	       state = RUNESTABLISHED;
	       printf("  %d sample run found beggining at sample %d\n", 
		     runCount, sampleCount - runCount);
	    }
	 break;

	 case RUNESTABLISHED:
            // expected = ((USHORT) ((USHORT) LastReceived + (USHORT) 1));
            expected = add(LastReceived, 1);

	    if(received == expected)
	       runCount++;
	    else {
	       state = REESTABLISHRUN;
	       printf("%d run ended at sample %d (expected %d, received %d)\n",
		     runCount, sampleCount, expected, received);
	    }
	 break;

	 case REESTABLISHRUN:
	 break;

	 case ERROR: 
            // here if received != expected (which is LastReceived + 1)

            ErrorCount++;
            printf("error detected: Received=%d LastReceived=%d expected=%d\n",
                    received, LastReceived, expected);

            // determine which kind of error

            if (received == LastReceived)
               // received the same value as PREVIOUS value
               ErrorCountRepeatedValue++;
      
            else if (received > LastReceived + 1)
               // (received - LastReceived) samples were skipped 
               ErrorCountSkippedValue++;

            else // sample is unexpected, but less than expected
                 // sample is less than or equal too the expected

               ErrorCountWrongValue++;

            // reset and try again
            state = ESTABLISHRUN;
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

   printf("Detected %d errors.\n", ErrorCount);
   printf("Detected %d missing values.\n", ErrorCountSkippedValue);
   printf("Detected %d repeated values.\n", ErrorCountRepeatedValue);
   printf("Detected %d incorrect values.\n", ErrorCountWrongValue);
}
