// *********************************************************************
// Function 1. Slow Inverse Fourier Transform

void ISFT(float T[], float C[], float S[], int N)
/* T[] is the output time sample array, N elements, initially zeros
   C[] is the input array of cosine amplitudes, (N/2)+1 elements
   S[] is the input array of sine amplitudes, (N/2)+1 elements
   N   is number of samples in the output record
*/ 
{   float ang, angstep;    
    float cval, sval;
    int   i,j;
    angstep=6.2831853/N;
    for(i=0; i<=N/2; i++)
    {   cval=C[i];
        sval=S[i];
        for(j=0; j<N; j++)
        {   ang = angstep*i*j;
            T[j] +=  cval*cos(ang)+sval*sin(ang);
        }
    }
    return;
}

// *********************************************************************
// Function 2. Slow Forward Fourier Transform

void FSFT(float T[], float C[], float S[], int N)
/* T[] is the input time sample array, N elements
   C[] is the output array of cosine amplitudes, (N/2)+1 elements
   S[] is output array of sine amplitudes, (N/2)+1 elements
   N   is number of samples in the input record
*/ 
{   float ang, angstep;
    float Nover2;
    float csum, ssum;
    int   i,j;
    angstep=6.2831853/N;
    Nover2=N/2.;
    for(i=0; i<=N/2; i++)
    {   csum=0;
        ssum=0;
        for(j=0; j<N; j++)
        {   ang = angstep*i*j;
            csum += T[j]*cos(ang);
            ssum += T[j]*sin(ang);
        }
        C[i]=csum/Nover2;   /* Correct the amplitude */
        S[i]=ssum/Nover2;
    }
    C[0] /= 2.;             /* Further correct the DC amplitude */
    C[N/2] /= 2.;           /* Further correct the Nyquist amplitude */
    return;
}

// *********************************************************************
// Function 3. Complex Fast Fourier Transform

void CFFT(int K, int DIR, float D1[], float D2[])
/* K    is the FFT order = log2(<size of Dx[] arrays>)
        Range is 2 to 15 inclusive.  Change declarations for N and i1-i8 to
        long if you wish to transform records longer than 32768 complex 
        samples.
   DIR  is +1 for forward transform (time->freq), -1 for inverse transform
   D1[] is the real parts of the input and output array, 2^K elements
   D2[] is the imaginary parts of the input and output array, 2^K elements
*/ 
{   float ang;                     /* Temporary angle */
    float t1,t2;                   /* Temporary D1, D2, array readout */
    float c,s;                     /* cosine & sine multipliers */
    unsigned int N;                /* Size of D1, D2 arrays */
    unsigned int i1,i2,i3,i4,i6,i7,i8;    /* Temporary indices */

    N = 1<<K;             /* Calcualte complex record size from K */
    i1=0;                 /* Bit reverse scramble the input data order */
    i2=N-1;
    for(i3=1; i3<N; i3++) /* i3 is normal counter, i1 is bit rev counter */
    {   i4=N;
        while(i1+i4 > i2)  i4 >>= 1;
        i1 = (i1%i4)+i4;
        if(i1 > i3)
        {   t1=D1[i3];
            D1[i3]=D1[i1];
            D1[i1]=t1;
            t2=D2[i3];
            D2[i3]=D2[i1];
            D2[i1]=t2;
        }
    }
    i4=1;                           /* Perform the complex transform */
    while(i4<N)
    {   i6=i4<<1;
        for(i3=0; i3<i4; i3++)
        {   ang=i3*3.1415927/i4;
            c=cos(ang);
            if (DIR>=0) s=sin(ang);
            else s=-sin(ang);
            for(i7=i3; i7<N; i7+=i6)
            {   i8=i7+i4;
                t1=c*D1[i8]-s*D2[i8];
                t2=c*D2[i8]+s*D1[i8];
                D1[i8]=D1[i7]-t1;
                D2[i8]=D2[i7]-t2;
                D1[i7]+=t1;
                D2[i7]+=t2;
            }
        }
        i4=i6;
    }
    return;
}

// *********************************************************************
// Function 4. Complex to Real Conversion for Forward Real Transform

void RealT(int K, float D1[], float D2[])
/* This function is to be used after calling CFFT when performing a
   forward transform of real data.
   K is the FFT order = log2(<size of Dx[] arrays>).  
     Range is 2 to 15 inclusive.  Change declarations for N, Nover2, i1, i2
     to long if you wish to transform records longer than 65536 real samples.
   The number of real samples transformed is 2*2^K.
   D1[], D2[] is the complex spectrum result from CFFT that is input to
   this function.  On return, D1 is the cosine amplitudes of the
   spectrum and D2 is the sine amplitudes.  D1[0] contains the DC
   component and D2[0] contains the Nyquist component.
   The function also does amplitude correction so that the final 
   spectrum output is the true spectrum of the input to CFFT.
*/
{   float t1,t2,t3,t4,t5,t6;  /* Temps for efficient calculation */
    float ang;                /* Temporary angle */
    float c,s;                /* cosine & sine multipliers */
    float Ntimes2;            /* =N*2 */
    unsigned int Nover2;      /* =N/2 */
    unsigned int N;           /* Size of D1 and D2 arrays */
    unsigned int i1,i2;       /* Temporary indices */

    N = 1<<K;                 /* Calcualte complex record size from K */
    Ntimes2=N*2.;
    Nover2=N/2;          

    t1=D1[0];                 /* Compute, correct, and place DC component */
    t2=D2[0];                 /* Compute, correct, and place Nyquist */
    D1[0]=(t1+t2)/Ntimes2;
    D2[0]=(t1-t2)/Ntimes2;    // HTB -- Doesn't look right...

    for(i1=1; i1<=Nover2; i1++) /* Compute remainder of frequency components */
    {   i2=N-i1;
        ang=3.1415927*i1/N;
        c=cos(ang);
        s=-sin(ang);
        t1=D1[i1]+D1[i2];
        t2=D1[i1]-D1[i2];
        t3=D2[i1]+D2[i2];
        t4=D2[i1]-D2[i2];
        t5=t2*s-t3*c;
        t6=t2*c+t3*s;
        D1[i1]=(t1-t5)/Ntimes2;
        D1[i2]=(t1+t5)/Ntimes2;
        D2[i1]=(t4-t6)/Ntimes2;
        D2[i2]=(-t4-t6)/Ntimes2;
    }
    return;
}

// *********************************************************************
// Function 5. Real to Complex Conversion for Inverse Real Transform

void CplxT(int K, float D1[], float D2[])
/* This function is to be used before calling CFFT when performing
   an inverse transform to produce real data.
   K is the FFT order = log2(<size of Dx[] arrays>).  
     Range is 2 to 15 inclusive.  Change declarations for N, Nover2, i1, i2
     to long if you wish to transform records longer than 65536 real samples.
   The number of real samples produced is 2*2^K.
   D1[] and D2[] is the input spectrum to this function.
   D1 is the cosine amplitudes and D2 is the sine amplitudes.
   D1[0] should contain the DC component and D2[0] the Nyquist component.
   The complex spectrum produced is then processed by CFFT to produce
   real samples stored alternately in D1 and D2.
*/
{   float t1,t2,t3,t4,t5,t6;  /* Temps for efficient calculation */
    float ang;                /* Temporary angle */
    float c,s;                /* cosine & sine multipliers */
    unsigned int N;           /* Size of D1 and D2 arrays */
    unsigned int Nover2;      /* =N/2 */
    unsigned int i1,i2;       /* Temporary indices */

    N = ldexp(1.,K);          /* Calcualte complex record size from K */
    t1=D1[0];                 /* Restore DC and Nyquist components */
    t2=D2[0];
    D1[0]=t1+t2;
    D2[0]=t1-t2;

    Nover2=N/2;          /* restore remainder of frequency components */
    for(i1=1; i1<=Nover2; i1++)
    {   i2=N-i1;
        ang=-3.1415927*i1/N;
        c=cos(ang);
        s=sin(ang);
        t1=D1[i1]+D1[i2];
        t4=D2[i1]-D2[i2];
        t5=D1[i2]-D1[i1];
        t6=-D2[i1]-D2[i2];
        t2=t5*s+t6*c;
        t3=t6*s-t5*c;
        D1[i1]=(t1+t2)/2.;
        D1[i2]=(t1-t2)/2.;
        D2[i1]=(t3+t4)/2.;
        D2[i2]=(t3-t4)/2.;
    }
    return;
}

// *********************************************************************

/*----------------------------------------------------------------------
 * Functions ffa & ffsy added by bunnell as wrappers to the Chamberline FFT
 * functions.
 *----------------------------------------------------------------------
 */
#include <math.h>

void ffa(float *x, int ns)
{
  int order, j, k, nso2;
  float *re, *im;

  order = log2(ns) - 1;
  nso2 = ns/2;
  re = (float *) calloc(nso2, sizeof(float));
  im = (float *) calloc(nso2, sizeof(float));
  for (j=0, k=0; j<ns; j+=2, k++) {
    re[k] = x[j];
    im[k] = x[j+1];
  }
  CFFT(order, +1, re, im);
  RealT(order, re, im);
  x[0] = ns * re[0];
  x[1] = 0.0;
  x[ns] = ns * im[0];
  x[ns+1] = 0.0;
  for (j=1, k=2; j<nso2; j++, k+=2) {
    x[k] = ns * re[j];
    x[k+1] = ns * im[j];
  }
  free(re);
  free(im);
  return;
}

void ffsy(float *x, int ns)
{
  int order, j, k, nso2;
  float *re, *im;

  order = log2(ns) - 1;
  nso2 = ns / 2;
  re = (float *) calloc(nso2, sizeof(float));
  im = (float *) calloc(nso2, sizeof(float));
  re[0] = x[0] / ns;
  im[0] = x[ns] / ns;
  for (j=2, k=1; j<ns; j+=2, k++) {
    re[k] = x[j] / ns;
    im[k] = x[j+1] / ns;
  }
  CplxT(order, re, im);
  CFFT(order, -1, re, im);
  for (j=0, k=0; j<nso2; j++, k+=2) {
    x[k] = re[j];
    x[k+1] = im[j];
  }
  free(re);
  free(im);
  return;
}

