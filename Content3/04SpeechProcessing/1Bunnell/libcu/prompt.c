/*
Name		Prompt - prompt for input from stdin

Synopsis	prompt ("prompt text", return_string)
		char return_string[];	user response to prompt

Description	Prints prompt text on the standard output and accepts 
		a string response which is returned to calling program.
		The '\n' character is trimmed from the return string.
		The calling program must ensure that there is sufficient
		space to hold the response string.

*/
#include <stdio.h>
#include "libcu.h"

void DLL_PREFIX prompt (char *p, char *r)
{
    int c;
    while ((c = *(p++)) != '\0')
	putchar(c);
    while ((*(r++) = getchar()) != '\n') {}
    *(--r) = '\0';
}
