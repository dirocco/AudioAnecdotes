/*
Name		DCS - Delimiting Character Scan

Synopsis	nmoved = dcs (dc, ipt, ins, ous, ch)
		char *dc, *ins, *ous, *ch;
		int  *ipt, nmoved;

Description	Move characters from "ins" starting at offset "ipt" to "ous".
		Stop transferring characters when one of the delimiter 
		characters pointed to by "dc" is encountered.  The delimiter
		character is returned in "ch", the number of characters
		transferred is the function return, and "ipt" is updated to
		point to the first character after the delimiter in "ins".

*/
#include <string.h>
#include "libcu.h"

#define DQUOTE '\"'
#define SQUOTE '\''

int DLL_PREFIX dcs (char *dc, int *ipt, char *ins, char *ous, char *ch)
{
    int i;
    int ndc, nou, c;
    int litc = '\0';    /* Literal character when one is in effect */
    ndc = strlen(dc);    /* number of delimiters to check for */
    nou = 0;	/* number of chars transferred to output */

    while ((c = *(ins + *ipt)) != '\0')
	{
	if (litc)
	/* In the scope of a literal sequence */
	    {
	    if (c == litc)
		litc = '\0';

	    /* continue with literal */
	    else
		{
		*(ous++) = c;
		nou++;
		}
	    }
	else
	/* Literal not in effect */
	    {
	    if ((c == SQUOTE) || (c == DQUOTE))
	    /* start literal sequence */
		litc = c;   
    
	    else
	    /* process in normal fashion */
		{
		if ((c != ' ') || (nou != 0))
		    {
		    for (i=0; i < ndc; i++)
			if (*(dc+i) == c)
			    {
			    *ch = c;
			    *ous = '\0';
			    if (c != '\0') (*ipt)++;
			    return(nou);
			    }
		    *(ous++) = c;
		    nou++;
		    }
		}
	    }
	(*ipt)++;
	}
    *ch = '\0';
    *ous = '\0';
    return(nou);
    }
