#include <sys/types.h>
#include <sys/stat.h>
#include "libcu.h"

#if defined( MSC ) || defined ( MSC16 )
#include <io.h>
#include <malloc.h>
#define OLDOPENBITS O_BINARY|O_RDWR
#define NEWOPENBITS O_BINARY|O_RDWR|O_CREAT|O_TRUNC
#define S_IRALL S_IREAD
#define S_DEFAULT S_IREAD|S_IWRITE
#else
#include <unistd.h>
#define OLDOPENBITS O_RDWR
#define NEWOPENBITS O_RDWR|O_CREAT|O_TRUNC
#define S_IRALL S_IRUSR|S_IRGRP|S_IROTH
#define S_DEFAULT S_IRALL|S_IWUSR|S_IWGRP
#endif

#define L_SET 0

#include <fcntl.h>
#include <stdio.h>


DLL_PREFIX RFLFILE *opnrfl( char *name, char mode, RFLDB *db)
{
    int SFLAGS, OFLAGS, handle;
    float tempf1;
    RFLFILE *rfp;

    if (mode == 'o' || mode == 'O') {
      OFLAGS = OLDOPENBITS;
      SFLAGS = 0;
    } else {
      OFLAGS = NEWOPENBITS;
      SFLAGS = S_DEFAULT;
    }

    if ((handle = open (name, OFLAGS, SFLAGS)) < 0)
	return (RFLFILE *) 0;

    rfp = (RFLFILE *) malloc(sizeof(RFLFILE));

    if (mode == 'o' || mode == 'O') {
	read (handle, &db->wleng, 4);
	read (handle, &db->step, 4);
	read (handle, &db->srate, 4);
	read (handle, &tempf1, 4);
	db->m = (int) tempf1;
	rfp->flag = 0;
    } else {
	tempf1 = (float) db->m;
	write (handle, &db->wleng, 4);
	write (handle,&db->step, 4);
	write (handle, &db->srate, 4);
	write (handle, &tempf1, 4);
	rfp->flag = 1;
    }

    rfp->handle = handle;
    rfp->rec_loc = 16;
    rfp->rec_len = 4 * (db->m + 2);
    rfp->time = 0.0f;
    rfp->step = db->step;

    return rfp;
}

int DLL_PREFIX rflin( RFLFILE *rfp, RFLREC *rec, float time)
{
    long nin, rnum;
    
    rnum = (long) (time / rfp->step + 0.5);

    if (time != rfp->time && time >= 0.0) {
/*
 *  Need to reposition the file pointer before reading
 */
	rfp->rec_loc = 16L + (long) rfp->rec_len * rnum;
	nin = (int) lseek(rfp->handle, rfp->rec_loc, L_SET);
	if (nin != rfp->rec_loc) return -1;
	rfp->time = time;
    }

    nin = read (rfp->handle, rec, (unsigned int) rfp->rec_len);
    if (nin < rfp->rec_len) return -1;
    if (rec->time == -1.0) return -1;
    if (rec->time != (rfp->step * (float) rnum))
	    return -1;
    rfp->rec_loc += rfp->rec_len;
    rfp->time += rfp->step;
    return 0;
}

int DLL_PREFIX rflou( RFLFILE *rfp, RFLREC *rec, float time)
{
    long nin, rnum;

    if (time != rfp->time && time >= 0.0) {
/*
 *  Need to reposition the file pointer before writing
 */
	rnum = (int) (time / rfp->step + 0.5);
	rfp->rec_loc = 16L + (long) rfp->rec_len * rnum;
	nin = lseek(rfp->handle, rfp->rec_loc, L_SET);
	if (nin != rfp->rec_loc) return -1;
	rfp->time = rfp->step * (float) rnum;
    }

    rec->time = rfp->time;
    nin = write (rfp->handle, rec, (unsigned int) rfp->rec_len);
    if (nin < rfp->rec_len) return -1;
    rfp->rec_loc += rfp->rec_len;
    rfp->time += rfp->step;
    return 0;
}

int DLL_PREFIX clsrfl(RFLFILE *rfp)
{
    if (rfp->flag) {
	float minus1 = -1.0f;
	write (rfp->handle, (char *) &minus1, sizeof(float));
    }

    close(rfp->handle);
    free(rfp);
    return 0;
}
