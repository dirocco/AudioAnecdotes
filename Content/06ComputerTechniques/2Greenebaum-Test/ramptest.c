#define THRESHOLD 5

main()
{
   typedef unsigned short USHORT;

   USHORT count = 0;
   int result;

   USHORT Received = 0, LastReceived = 0;
   int    SignalCount = 0;
   int    Total = 0;
   int    ErrorCount = 0;
   USHORT Expected;


   enum {START, WAITFORSIGNAL, RECEIVEDSIGNAL, ESTABLISHRUN, 
                               RUNESTABLISHED, ERROR } states;
   states state;

   while(1) 
   {
      LastReceived = Received;
      result = scanf("%d", &Received);
      if (result != 1)
      {
        printf("scanf didn't return 1\n");
        exit(-1);
      }
      Total++;

      switch(state) 
      {
	 case START:  // wait for (nonzero) signal
	    if (Received) 
            {
               count++;
	       printf("found signal at sample %d\n", count-2);
	       state = RECEIVEDSIGNAL;
	    }
            else
            {
               state = WAITFORSIGNAL;  // redundant, added for clarity.
               count = 0;  // redundant, added for clarity.
            }
	    break;

	 case WAITFORSIGNAL: 

            Expected =  ((USHORT) ((USHORT) LastReceived + (USHORT) 1));

	    if(Received == Expected)
            {
	       printf("%d %d %d ", Received, LastReceived, 
                          (USHORT) ((USHORT) LastReceived+ (USHORT) 1));
	       printf("received expected value at %d\n", Total - 1);
               // if we meet the theshold then transition to state 2
               if ( ++count > THRESHOLD)
               {
                  state = RUNESTABLISHED;
                  break;
               }

               state = 1;  // redundant, but put here for consistency
            }

	    else // Received != Expected
            {
               count = 0;  // reset count, proceed to error state (2)

	       printf("state 1: %d %d %d ", Received, LastReceived, 
                          (USHORT) ((USHORT) LastReceived+ (USHORT) 1));
	       printf("Received unexpected value at %d\n", Total - 1);
               state = ERROR;
            }
	    break;

		 
	 case ERROR: 

            ErrorCount++;
            printf("error detected\n");

  // not sure what to do here ... talk to ken again about
  // which errors to handle

            Expected = Received + 1; // fix this
            
	    break;

	 case ESTABLISHRUN: 
            Expected =  ((USHORT) ((USHORT) LastReceived + (USHORT) 1));
	    printf("state 3: %d %d %d ", Received, LastReceived, 
                          (USHORT) ((USHORT) LastReceived+ (USHORT) 1));
	    printf("received expected value at %d\n", Total - 1);
	    if(Received == Expected)
            {
              count++;
              state = ESTABLISHRUN;  // redundant but included for clarity
            }
            else 
            {
              count = 0;
              state = WAITFORSIGNAL;
            } 

	    break;


	 default:
	    printf("I fell and I can't get up\n");
      }
   }
   printf("Detected %d errors.\n", ErrorCount);
}
