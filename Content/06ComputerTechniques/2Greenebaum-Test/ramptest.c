#define THRESHOLD 5

main()
{
   typedef unsigned short USHORT;

   char state = 0;
   USHORT count = 0;
   int result;

   USHORT Received = 0, LastReceived = 0;
   int    SignalCount = 0;
   int    Total = 0;
   int    ErrorCount = 0;
   USHORT Expected;


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
	 case 0:  // wait for (nonzero) signal
	    if (Received) 
            {
               count++;
	       printf("found signal at sample %d\n", count-2);
	       state = 1;
	    }
            else
            {
               state = 0;  // redundant, added for clarity.
               count = 0;  // redundant, added for clarity.
            }
	    break;

	 case 1: 

            Expected =  ((USHORT) ((USHORT) LastReceived + (USHORT) 1));

	    if(Received == Expected)
            {
	       printf("state 1: %d %d %d ", Received, LastReceived, 
                          (USHORT) ((USHORT) LastReceived+ (USHORT) 1));
	       printf("received expected value at %d\n", Total - 1);
               // if we meet the theshold then transition to state 2
               if ( ++count > THRESHOLD)
               {
                  state = 3;
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
               state = 2;
            }
	    break;

		 
	 case 2: 

            ErrorCount++;
            printf("state 2: error detected\n");

  // not sure what to do here ... talk to ken again about
  // which errors to handle

            Expected = Received + 1; // fix this
            
	    break;

	 case 3: 
            Expected =  ((USHORT) ((USHORT) LastReceived + (USHORT) 1));
	    printf("state 3: %d %d %d ", Received, LastReceived, 
                          (USHORT) ((USHORT) LastReceived+ (USHORT) 1));
	    printf("received expected value at %d\n", Total - 1);
	    if(Received == Expected)
            {
              count++;
              state = 3;  // redundant but included for clarity
            }
            else 
            {
              count = 0;
              state = 1;
            } 

	    break;


	 default:
	    printf("I fell and I can't get up\n");
      }
   }
   printf("Detected %d errors.\n", ErrorCount);
}
