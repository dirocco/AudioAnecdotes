/**
Name		strpad - Pad spaces onto a string

Synopsis	strpad (str, length)
		void strpad;
		char *str;
		int length;

Description	Pads spaces out to length in str.
**/
#include "libcu.h"

void DLL_PREFIX strpad (char *str, int length)
{
  int i;

  for (i = 0; i < length; i++) {
    if (*str == '\0') {
      *str++ = ' ';
      *str = '\0';
    } else
      str++;
  }
  *str = '\0';
}
