main()
{
typedef unsigned short USHORT;

USHORT value = 0, lastval = 0;
char state = 0;
USHORT count = 0;

   while(1) {
      lastval = value;
      scanf("%d", &value);
      count++;

      switch(state) {
	 case 0:  // wait for signal
	    if(value) {
	       printf("found signal at sample %d\n", count-2);
	       state = 1;
	    }
	 break;

	 case 1: 
	    if(value != ((USHORT)((USHORT)lastval+(USHORT)1)))
	       printf("%d %d %d ", value, lastval, (USHORT)((USHORT)lastval+(USHORT)1)),
	       printf("we have a problem at %d\n", count -1);
	 break;
		 
	 default:
	    printf("I fell and I can't get up\n");
      }


   }



}
