#ifndef __UNPACK_HEADER__
#define __UNPACK_HEADER__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cmdarg {
  char *keyword;
  short type;
  char *value;
} CMDARG;

DLL_PREFIX void unpack(int argc, char **argv, CMDARG *cmdargs, char **cmdfiles);
DLL_PREFIX char *garg(CMDARG *cmdargs, char *key);

#ifdef __cplusplus
}
#endif

#endif
