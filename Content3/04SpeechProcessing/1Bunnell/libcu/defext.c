#include <string.h>
#include "libcu.h"

void DLL_PREFIX defext (char *str, char *ext)
{
	char *s;
	int nc, i;

	nc = strlen(str);
	s = str + nc - 1;
	for (i=0; i<nc; i++) {
		if (*s == '.')
			return;
		else if (*s == '\\' || *s == '/') {
			(void) strcat(str,".");
			(void) strcat(str, ext);
			return;
		}
		--s;
	}
    (void) strcat(str,".");
    (void) strcat(str, ext);
    return;
}
