#define THRESHOLD 5

main()
{
   typedef unsigned short USHORT;

   USHORT count = 0;        // length of the current run
   int result;              // used to test result of scanf

   USHORT Received = 0;     // stores value received from input stream
   USHORT LastReceived = 0; // stores previous value received
   int    SignalCount = 0;  
   int    Total = 0;        // total number of input values read
   int    ErrorCount = 0;   // total number of errors detected
   int    ErrorCountRepeatedValue = 0;
   int    ErrorCountSkippedValue  = 0;
   int    ErrorCountWrongValue    = 0;
   USHORT Expected;         // value expected to be seen in input stream


   typedef enum 
   {
         START,             // start state
         WAITFORSIGNAL,     // state for waiting for signal (nonzero value)
         RECEIVEDSIGNAL,    // state that indicates a signal is detected
         ESTABLISHRUN, 
         RUNESTABLISHED, 
         ERROR 
   } states ;

   states state = START;

   while(1) 
   {
      if (state != ERROR)
      {
         LastReceived = Received;
         result = scanf("%d", &Received);
         if (result != 1)
         {
            printf("scanf didn't return 1\n");
            break;
         }
         Total++;
      }

      switch(state) 
      {
	 case START:  // wait for (nonzero) signal
	    if (Received) 
            {
               count++;
	       printf("START: found signal at sample %d\n", count-2);
	       state = RECEIVEDSIGNAL;
	    }
            else
            {
               state = WAITFORSIGNAL;  // redundant, added for clarity.
               count = 0;  // redundant, added for clarity.
            }
	    break;

	 case WAITFORSIGNAL: 

            if (Received == 0)
            {
               state = WAITFORSIGNAL;
               break;
            }

            Expected =  ((USHORT) ((USHORT) LastReceived + (USHORT) 1));

	    if(Received == Expected)
            {
	       printf("WAITFORSIG received %d, lastreceived %d, expected %d ", 
                                Received, LastReceived, Expected);
	       printf("received expected value at %d\n", Total - 1);

               count++;
               state = ESTABLISHRUN;  // redundant, here for consistency
            }

	    else // Received != Expected
            {
               count = 0;  // reset count, proceed to error state (2)

	       printf("WAITFORSIG received %d, lastreceived %d, expected %d ", 
                                Received, LastReceived, Expected);
	       printf("Received unexpected value at %d\n", Total - 1);
               state = ERROR;
            }
	    break;
		 
	 case ESTABLISHRUN: 
            Expected =  ((USHORT) ((USHORT) LastReceived + (USHORT) 1));
            printf("ESTABLISHRUN received %d, lastreceived %d, expected %d ", 
                                Received, LastReceived, Expected);
	    printf("Received expected value at %d\n", Total - 1);

	    if(Received == Expected)
            {
              count++;
              state = ESTABLISHRUN;  // redundant but included for clarity
            }

            else 
            {
              count = 0;
              state = ERROR;
            } 

	    break;

	 case ERROR: 
            // here if Received != Expected (which is LastReceived + 1)

            ErrorCount++;
            printf("error detected: Received=%d LastReceived=%d Expected=%d\n",
                    Received, LastReceived, Expected);

            // determine which kind of error

            if (Received == LastReceived)
               // received the same value as PREVIOUS value
               ErrorCountRepeatedValue++;
      
            else if (Received > LastReceived + 1)
               // (Received - LastReceived) samples were skipped 
               ErrorCountSkippedValue++;

            else // sample is unexpected, but less than expected
                 // sample is less than or equal too the expected

               ErrorCountWrongValue++;

            // reset and try again
            state = ESTABLISHRUN;
	    break;

	 default:
	    printf("I fell and I can't get up\n");
      }
   }
   printf("Detected %d errors.\n", ErrorCount);
   printf("Detected %d missing values.\n", ErrorCountSkippedValue);
   printf("Detected %d repeated values.\n", ErrorCountRepeatedValue);
   printf("Detected %d incorrect values.\n", ErrorCountWrongValue);
}
