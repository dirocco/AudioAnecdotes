#include <stdio.h>
#include <string.h>
#include "libcu.h"

int match (char *str1, char *str2)
{
  while (*str2) if (*str1++ != *str2++) return 0;
  return 1;
}


DLL_PREFIX void unpack(int argc, char **argv, CMDARG *cmdargs, char **cmdfiles)
{
  int	j=0,
	k=0;

  int looking, nf=0;

  char *colon;

  while(++j < argc) {
      if (*argv[j] == '-') {
	  k=0;
	  argv[j]++;
	  looking = 1;
	  while ((cmdargs[k].keyword[0] != '\0') && looking) {
	      if (match(argv[j], cmdargs[k].keyword)) {
		  if (cmdargs[k].type) {
		      colon = strchr(argv[j], ':');
		      if (colon == (char *) 0) {
			  cmdargs[k].value = argv[++j];
		      } else {
			  cmdargs[k].value = ++colon;
		      }
		  } else {
		      cmdargs[k].value = argv[j];
		  }
		  looking = 0;
	      }
	      k++;
	  }
	  if (looking) 
	    fprintf(stderr, "unpack: unrecognized option: %s\n", argv[j]);
      } else {
	  cmdfiles[nf++] = argv[j];
      }
  }
  cmdfiles[nf] = (char *) 0;
}

DLL_PREFIX char *garg(CMDARG *cmdargs, char *key)
{
  int k=0, n=0;

  while ((*cmdargs[k].keyword != '\0')) {
      if (match(key, cmdargs[k].keyword))
	return cmdargs[k].value;
      k++;
  }
  return (char *) 0;
}

