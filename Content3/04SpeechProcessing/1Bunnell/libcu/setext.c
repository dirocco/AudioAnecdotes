#include <string.h>
#include "libcu.h"

void DLL_PREFIX setext(char *file, char *ext)
{
	int nf, ne, j;

	nf = strlen(file);
	ne = strlen(ext);
	for (j = nf - 1; j >= 0; j--) {
		if (file[j] == '.') {
			file[j+1] = (char) 0;
			strcat(file, ext);
			return;
		} else if (file[j] == '\\' || file[j] == '/') {
			file[nf] = '.';
			file[nf+1] = (char) 0;
			strcat(file, ext);
			return;
		}
	}
	file[nf] = '.';
	file[nf+1] = (char) 0;
	strcat(file, ext);
	return;
}
