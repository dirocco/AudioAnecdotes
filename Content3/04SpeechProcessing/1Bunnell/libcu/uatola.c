#include <ctype.h>
#include "libcu.h"

DLL_PREFIX char *uatola(char *s)
{
    char *sp;
    int c;
    sp = s;
    while (c = *sp) {
	if (isupper(c)) *sp = (char) tolower(c);
	else *sp = (char) c;
	sp++;
    }
    return s;
}

DLL_PREFIX char *latoua(char *s)
{
    char *sp;
    int c;
    sp = s;
    while (c = *sp) {
	if (islower(c)) *sp = (char) toupper(c);
	else *sp = (char) c;
	sp++;
    }
    return s;
}
