#include "libcu.h"

DLL_PREFIX char *stripext(char *fn)
{
	int j, k;
/*
 * For our purposes, an extension is any part of a file basename after a period.
 * In case the filename is part of a longer pathname, we stop the search if we 
 * run into a path delimiter before we find a period. This function treats either
 * forward or backward slash as a path delimiter.
 */
    j = strlen(fn);
    for(k=j-1; k>=0; k--) {
        if(fn[k]=='.')   {
            fn[k]='\0';
            return fn;
		} else if (fn[k] == '\\' || fn[k] == '/') {
			return fn;
		}
    }
    return fn;
}
