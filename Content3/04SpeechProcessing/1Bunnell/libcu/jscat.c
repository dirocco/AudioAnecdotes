/*---------------------------------------------------------------------------
 * Name		jscat - string concatenation routine
 *
 * Synopsis	rtnstr = jscat(rtnstr, str1, str2, ..., (char *) 0);
 *		char *rtnstr, *str1, *str2, ...;
 *
 * Description	Concatenates an arbitrary number of input strings to form
 *		a single output string.  The last last argument must be
 *		a NULL pointer. The function also returns a pointer to the
 *		return string as the value of the function. Results are
 *		undefined unless the arglist consists of at least one
 *		return string, one input string, and the NULL pointer.
 *---------------------------------------------------------------------------
 */

#include <string.h>
#include <varargs.h>
#include "libcu.h"

#define MAXARGS     100

DLL_PREFIX char *jscat (va_alist)
va_dcl
{
  va_list ap;
  char *outstr;
  char *str;
  int strno = 0;
  va_start (ap);
  outstr = va_arg(ap, char *);
  str = va_arg(ap, char *);
  (void) strcpy(outstr, str);
  while ((str = va_arg(ap, char *)) != (char *) 0)
	(void) strcat(outstr, str);
  va_end (ap);
  return outstr;
}

					 
