/********************************************************
//
//	DSP example function - Fourier transform
//
*********************************************************/

#include <stdio.h>		/* printf debug info         */
#include <stdlib.h>		/* EXIT_SUCCESS return value */
#include <string.h>
#include <math.h>

const int 	BUFFERSIZE = 64;

void ISFT(float T[], float C[], float S[], int N);
void FSFT(float T[], float C[], float S[], int N);
void CFFT(int K, int DIR, float D1[], float D2[]);
void RealT(int K, float D1[], float D2[]);
void CplxT(int K, float D1[], float D2[]);

void usage(char *argv)
{
   printf("Usage: %s [-f FFTORDER(2-15)] [-i(nverse)] [-s(low)] < samples\n", argv);
   exit(-1);
}

/*-----------------------------------------
Inputs:
	sample data set from standard in
Outputs:
	transformed samples to stdout
*///---------------------------------------
int main(int argc, char *argv[])
{
   int DIR=1;
   int COMPLEX=1;
   int FFTORDER=15;

   if (argc > 5)
      usage(argv[0]);

   for (int i = 1; i < argc; i++)
   {
      if (argv[i][0] != '-')
         usage(argv[0]);
      else if (argv[i][1] == 'f')
         FFTORDER = atoi(argv[++i]);
      else if (argv[i][1] == 'i')
         DIR = -1;
      else if (argv[i][1] == 's')
         COMPLEX = 0;
      else
         usage(argv[0]);
   }

   if (FFTORDER <  2) FFTORDER =  2;
   if (FFTORDER > 15) FFTORDER = 15;
   int WINDOWSIZE=(1 << FFTORDER);

   fprintf(stderr,"%s, %s, fft order %d, window size %d\n",
      DIR==1?"forward":"inverse",COMPLEX?"complex":"slow",FFTORDER,WINDOWSIZE);

   // size sample FIFO to transform window
   float samples[WINDOWSIZE],zeroes[WINDOWSIZE];
   float cosines[WINDOWSIZE/2+1],sines[WINDOWSIZE/2+1];

   //read samples in from stdin
   int i,j;
   do {
      char string[BUFFERSIZE];
      if (DIR > 0) {
         bzero(samples, sizeof(samples));
         for (i=0; fgets(string, BUFFERSIZE, stdin) && i < WINDOWSIZE; i++)
            sscanf(string,"%f\n",&samples[i]);
         if (COMPLEX) {
            bzero(zeroes, sizeof(zeroes));
            CFFT(FFTORDER, DIR, samples, zeroes);
            RealT(FFTORDER, samples, zeroes);
            for (j = 0; j < i/2+1; j++)
               printf("%f %f\n",samples[j],zeroes[j]);
         }
         else {
            FSFT(samples, cosines, sines, i);
            for (j = 0; j < i/2+1; j++)
               printf("%f %f\n",cosines[j],sines[j]);
         }
      }
      else {
         bzero(samples, sizeof(samples));
         if (COMPLEX) {
            bzero(zeroes, sizeof(zeroes));
            for (j=0; fgets(string, BUFFERSIZE, stdin) && j < WINDOWSIZE/2+1; j++)
               sscanf(string,"%f %f\n",&samples[j],&zeroes[j]);
            CplxT(FFTORDER, samples, zeroes);
            CFFT(FFTORDER, DIR, samples, zeroes);
         }
         else {
            for (j=0; fgets(string, BUFFERSIZE, stdin) && j < WINDOWSIZE/2+1; j++)
               sscanf(string,"%f %f\n",&cosines[j],&sines[j]);
            ISFT(samples, cosines, sines, (j-1)*2);
         }
         for (i = 0; i < (j-1)*2; i++)
            printf("%f\n",samples[i]);
      }
   } while (i == WINDOWSIZE);
}


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
    D2[0]=(t1-t2)/Ntimes2;

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
