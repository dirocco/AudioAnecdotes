#ifndef __RFLIO_HEADER__
#define __RFLIO_HEADER__

#ifdef __cplusplus
extern "C" {
#endif

#define MAXRC 36

typedef struct {
    int handle;
    long rec_loc;
    long rec_len;
    float time;
    float step;
    char flag;
} RFLFILE;

typedef struct {
    int m;
    float wleng, step, srate;
} RFLDB;

typedef struct {
    float time;
    float alpha;
    float rc[MAXRC];
} RFLREC;

DLL_PREFIX RFLFILE *opnrfl(char * name, char mode, RFLDB * data_block);
int       DLL_PREFIX rflin(RFLFILE * rfp, RFLREC * record, float time);
int       DLL_PREFIX rflou(RFLFILE * rfp, RFLREC * record, float time);
int      DLL_PREFIX clsrfl(RFLFILE * rfp);

#ifdef __cplusplus
}
#endif

#endif
