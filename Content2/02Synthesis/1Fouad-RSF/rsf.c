#include <math.h>
#include <stdlib.h>
#define XEXT 10
#define YEXT 10
#define ZEXT 10

typedef struct SoundRec
{
    int soundId;
    float pos[3];
    int activate;
    long nextactive;
    long j, p, o;
    struct SoundRec *next;
} SOUNDREC;

void Update(SOUNDREC *sound, long time)
{
    static long n=0;		
    SOUNDREC *sp = sound;
	
    while (sp != NULL)
    {
        if (sp->nextactive <= time)	
	  {
		// Calculate a random position within the volume 
 	      sp->pos[0] = 2*XEXT*drand48()- XEXT;
		sp->pos[1] = 2*YEXT*drand48()- YEXT;
		sp->pos[2] = 2*ZEXT*drand48()- ZEXT;
		// Set the activate flag to indicate that the sound should
		// be activated
		sp->activate = 1;
		// Calculate the next activation time for this sound
		sp->nextactive = (n - 1) * sp->p + sp->o - sp->j +
2 * sp->j * drand48();
		// Must avoid negative activation times
		if (sp->nextactive < 0)
		    sp->nextactive = 0;
	  }
  	  sp = sp->next;
    }
    n++;
}
