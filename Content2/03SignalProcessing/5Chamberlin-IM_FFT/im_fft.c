#include <stdio.h>
// ******************************************************************
// Function 1. Cosine Table Creator for ICFFT, IRealT, and ICplxT

void MakeICT(int Kmax, FILE *output_file)
/* Function to create an integer cosine table for ICFFT, IRealT, and ICplxT.
   Kmax specifies the table length to allow transforms up to 2^Kmax complex
   points.  Two data statements will be written to the file which can
   then be compiled into the user's application.  The first defines Kmax.
   Thje second defines the table, IcosT.  Its length is 2^Kmax/4+1.
   Maximum Kmax is 18, minimum is 5 for a valid table to be generated.
   This version creates a table with a maximum value of 65535.
*/

{   long N;                 /* Table size is N = 2^Kmax/4 +1 */
    long t;                 /* Table entry */
    long i,j;               /* Counters */

    N = 1<<(Kmax-2);        /* Calculate table size from Kmax */

    fprintf(output_file,"/* Cosine table for 0 - PI/2 for FFTs up to %lu ",N*4);
    fprintf(output_file,"complex points. */\r\n");
    fprintf(output_file,"int Kmax=%i;\r\n",Kmax); 
    fprintf(output_file,"unsigned int IcosT[%i] = {\r\n",N+1); 
    for (i=0; i<N; i+=8)
    {   fprintf(output_file,"  ");  /* Leading spaces at beginning of a line */
        for (j=0; j<8; j++)
        {   t=65535*cos((i+j)*1.570796327/N)+0.5;
            fprintf(output_file,"%lu,",t);
        }
        fprintf(output_file,"\r\n");              /* End of line of 8 values */
    }
    fprintf(output_file,"  0};\r\n"); /* Last entry is zero then end of table */
    return;
}

// ******************************************************************
// Function 2. Complex fast Fourier transform using integer arithmetic

void ICFFT(unsigned int IcosT[], int Kmax, int K, int DIR, int D1[], int D2[])
/* Complex fast Fourier transform using only integer arithmetic
   IcosT is cosine table for 0 through pi/2; unsigned entries 0-65535
   Kmax  indicates the length of the cosine table: (2^Kmax/4+1) entries
   K     is the FFT order = log2<size of Dx[] arrays>
         Range is 2 to Kmax inclusive.  
   DIR   is +1 for forward transform (time->freq), -1 for inverse transform
   D1[]  is the real parts of the input and output array, 2^K elements
   D2[]  is the imaginary parts of the input and output array, 2^K elements
*/ 
{   unsigned int N, Nover2;        /* Size of Dx arrays, half that */
    int Tish;                      /* Cosine table index shift */
    unsigned int i1,i2,i3,i4,i6,i7,i8;    /* Temporary indices */
    unsigned int  ang;             /* Temps for cosine */
    unsigned long angtemp;         /* and sine lookup */
    unsigned int c,s;              /* cosine & sine magnitudes */
    int cs,ss;                     /* consine & sine signs (1=negative) */
    int td1,td2;                   /* Temporary D1, D2, array readout */
    long t1,t2,t3,t4;              /* Long temporaries */

    N = 1<<K;             /* Calculate complex record size from K */
    Nover2 = N>>1;        /* N/2 */
    Tish = Kmax-K;        /* Shift amount for cosine table lookup index */
    i1=0;                 /* Scramble the input data into bit reverse order */
    i2=N-1;
    for(i3=1; i3<N; i3++) /* i3 is normal counter, i1 is bit reverse counter */
    {   i4=N;
        while(i1+i4 > i2)  i4 >>= 1;
        i1 = (i1%i4)+i4;
        if(i1 > i3)
        {   td1=D1[i3];
            D1[i3]=D1[i1];
            D1[i1]=td1;
            td2=D2[i3];
            D2[i3]=D2[i1];
            D2[i1]=td2;
        }
    }
    i4=1;                               /* Perform the complex transform */
    while(i4<N)
    {   i6=i4<<1;
        for(i3=0; i3<i4; i3++)          /* For W function evaluation:     */
        {   angtemp=(long)i3*(long)N;   /* Express angle range of 0 - pi  */
            ang=angtemp/i4;             /* as index range of 0 - N        */
            if(ang<=Nover2)             /* For 0 <= ang <= pi/2 :      */
            {   c=IcosT[ang<<Tish>>1];  /* Use ang directly for cosine */
                cs=0;                   /* Sign of cosine is +         */
                s=IcosT[(Nover2-ang)<<Tish>>1];/* complement ang for sin */
            }
            else                               /* For pi/2 < ang <= pi :      */
            {   c=IcosT[(N-ang)<<Tish>>1];     /* for cosine, complement ang  */
                cs=1;                          /* and sign is -               */
                s=IcosT[(ang-Nover2)<<Tish>>1];/* Shift ang for sine     */
            }
            if(DIR<0) ss=1; else ss=0;    /* sine is + for forward, - for inv */
            for(i7=i3; i7<N; i7+=i6)
            {   i8=i7+i4;
                t1=(long)c*(long)D1[i8];  /* Cosine multiplications */
                t3=(long)c*(long)D2[i8];
                if(cs) { t1=-t1; t3=-t3; }
                t2=(long)s*(long)D1[i8];  /* Sine multiplications */
                t4=(long)s*(long)D2[i8];
                if(ss) { t2=-t2; t4=-t4; }
                t1=((t1>>1)-(t4>>1))>>15;  /* Final recombinations */
                t2=((t3>>1)+(t2>>1))>>15;

                if(DIR>=0)
                {   t3=D1[i7]-t1;          /* Forward transform requires */
                    D1[i8]=t3>>1;          /* scaling by 1/2 every butterfly */
                    t3=D2[i7]-t2;
                    D2[i8]=t3>>1;
                    t3=D1[i7]+t1;
                    D1[i7]=t3>>1;
                    t3=D2[i7]+t2;
                    D2[i7]=t3>>1;
                }
                else
                {   t3=D1[i7]-t1;          /* Inverse transform does not */
                    D1[i8]=t3;             /* require scaling */
                    t3=D2[i7]-t2;
                    D2[i8]=t3;
                    t3=D1[i7]+t1;
                    D1[i7]=t3;
                    t3=D2[i7]+t2;
                    D2[i7]=t3;
                }
            }
        }
        i4=i6;
    }
    return;
}

// ******************************************************************
// Function 3. Complex to Real Conversion using integer arithmetic

void IRealT(unsigned int IcosT[], int Kmax, int K, int D1[], int D2[])
/* This function is to be used after calling ICFFT when performing a
   forward transform of real data.  Only integer arithmetic is used.
   IcosT is cosine table for 0 through pi/2; unsigned entries 0-65535
   Kmax  indicates the length of the cosine table: (2^Kmax/4+1) entries
   K     is the FFT order = log2(<size of Dx[] arrays>).  
         Range is 2 to Kmax inclusive.  
         The number of real samples transformed is 2*2^K.
   D1[], D2[] is the complex spectrum result from ICFFT that is input to
   this function.  On return, D1 is the cosine amplitudes of the
   spectrum and D2 is the sine amplitudes.  D1[0] contains the DC
   component and D2[0] contains the Nyquist component.
   The function also does amplitude correction so that the final 
   spectrum output is the true spectrum of the input to CFFT.
*/
{   long t1,t2,t3,t4,t5,t6;   /* Temps for efficient calculation */
    int          Tish;        /* Cosine table index shift */
    unsigned int c,s;         /* cosine & sine magnitudes */
    unsigned int N;           /* Size of D1 and D2 arrays */
    unsigned int Nover2;      /* =N/2 */
    unsigned int i1,i2;       /* Temporary indices */

    N = 1<<K;                 /* Calcualte complex record size from K */
    Nover2=N/2;          
    Tish = Kmax-K-1;          /* Shift amount for cosine table lookup index */
    t1=D1[0];                 /* Compute, correct, and place DC component */
    t2=D2[0];                 /* Compute, correct, and place Nyquist */
    D1[0]=(t1+t2)>>1;
    D2[0]=(t1-t2)>>1;
    for(i1=1; i1<=Nover2; i1++) /* Compute remainder of frequency components */
    {   i2=N-i1;
        if(Tish >= 0)                   /* Standard case for K<Kmax */
        {   c=IcosT[i1<<Tish];          /* Direct table lookup since*/
            s=IcosT[(Nover2-i1)<<Tish]; /* 0 < angle <= pi/2        */
        }
        else                            /* Special case for K=Kmax  */
        {   if(i1%2)                    /* For i1 is odd, must interpolate */
            {   t1=IcosT[i1>>1];              /* Cosine */ 
                t2=IcosT[(i1>>1)+1];
                t3=(t1+t2)>>1;
                c=t3;
                t1=IcosT[(Nover2-i1)>>1];     /* Sine */
                t2=IcosT[((Nover2-i1)>>1)+1];
                t3=(t1+t2)>>1;
                s=t3;
            }
            else                        /* For even, just lookup */
            {   c=IcosT[i1>>1];
                s=IcosT[(Nover2-i1)>>1];
            }
        }
        t1=((long)D1[i1]+(long)D1[i2])>>1; /* Preliminary combinations */
        t2=((long)D1[i1]-(long)D1[i2])>>1;
        t3=((long)D2[i1]+(long)D2[i2])>>1;
        t4=((long)D2[i1]-(long)D2[i2])>>1;
        t5=-t3*c;                          /* Cosine multiplications */
        t6=t2*c;
        t5-=t2*s;                          /* Sine multiplications */
        t6-=t3*s;
        D1[i1]=((t1<<16)-t5)>>16;          /* Final combinations */
        D1[i2]=((t1<<16)+t5)>>16;    
        D2[i1]=((t4<<16)-t6)>>16;
        D2[i2]=((-t4<<16)-t6)>>16;
    }
    return;
}

// ******************************************************************
