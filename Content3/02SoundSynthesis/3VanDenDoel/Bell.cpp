#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <pablio.h>


/** Modal model, which is loaded from an .sy format text file.
        @author Kees van den Doel (kvdoel@cs.ubc.ca)
*/

/** Mode frequencies in Hertz. */
float *f;

/**  Angular decay rates in Hertz. */
float *d;

/** Gains. a[p][k] is gain at point p for mode k. */
float *a;

/** Number of modes available. */
int nf;

/** Number of modes used. */
int nfUsed;
    
/** Number of points. */
int np;

/** Multiplies all frequencies. */
float fscale;

/** Multiplies all dampings. */
float dscale;

/** Multiplies all gains. */
float ascale; 

/** Allocated arrays.
    @param nf number of modes.
    @param np number of locations.
*/
void allocateArrays(int nf, int np) {
    f = new float[nf];
    d = new float[nf];
    a = new float[np*nf];
    fscale = dscale = ascale = 1.f;
};

void readModes(FILE *fp) {
    float dval;
    int ival;
    char s[256];

    fscanf(fp, "%s\n", &s);
    // s is now: "nactive_freq:"    
    fscanf(fp, "%d\n", &nfUsed);
    //printf(":%s:\n",s);
    fscanf(fp, "%s\n", &s); // n_freq:
    fscanf(fp, "%d\n", &nf);
    fscanf(fp, "%s\n", &s); // n_points:
    fscanf(fp, "%d\n", &np);
    fscanf(fp, "%s\n", &s); // f_scale:
    fscanf(fp, "%f\n", &fscale);
    fscanf(fp, "%s\n", &s); // d_scale:
    fscanf(fp, "%f\n", &dscale);
    fscanf(fp, "%s\n", &s); // a_scale:
    fscanf(fp, "%f\n", &ascale);

    allocateArrays(nf,np);
            
    fscanf(fp, "%s\n", &s); // frequencies:
    for(int i=0;i<nf;i++) {
        fscanf(fp, "%f\n", &f[i]);
    }
    fscanf(fp, "%s\n", &s); // dampings:
    for(int i=0;i<nf;i++) {
        fscanf(fp, "%f\n", &d[i]);
    }
    fscanf(fp, "%s\n", &s); // amplitudes[point][freq]:
    for(int p=0;p<np;p++) {
        for(int i=0;i<nf;i++) {
            fscanf(fp, "%f\n", &a[p*nf+i]);
        }
    }
    fscanf(fp, "%s\n", &s); // END
};

/** Read the modes file in .sy format.
    @param fn File name with modal data in .sy format.
*/
void readModesFile(char *fn) {
    FILE *fp = fopen(fn, "r");
    readModes(fp);
    fclose(fp);
};


/** Vibration model of object, capable of playing sound.
    @author Kees van den Doel (kvdoel@cs.ubc.ca)
*/

/** Sampling rate in Hertz. */
float srate = 44100.f;

/** State of filters. */
float *yt_1, *yt_2; 
    
/** The transfer function of a reson filter is H(z) = 1/(1-twoRCosTheta/z + R2/z*z). */
float *R2;

/** The transfer function of a reson filter is H(z) = 1/(1-twoRCosTheta/z + R2/z*z). */
float *twoRCosTheta;

/** Cached values. */
float *c_i;

/** Reson filter gain vector. */
float *ampR;

/** Compute gains of contact
*/
void computeLocation() {
    for(int i=0;i<nf;i++) {
        ampR[i] = ascale * c_i[i] * a[i];
    }
}
    
/** Set state to non-vibrating.
*/
void clearHistory() {
    for(int i=0;i<nf;i++) {
        yt_1[i] = yt_2[i] = 0;
    }
}

/** Allocate data.
    @param nf number of modes.
    @param np number of locations.
*/
void allocateData(int nf,int np) {
    R2 = new float[nf];
    twoRCosTheta = new float[nf];
    yt_1 = new float[nf];
    yt_2 = new float[nf];
    c_i = new float[nf];
    ampR = new float[nf];
    clearHistory();
}

/** Compute the reson coefficients from the modal model parameters.
    Cache values for location computation.
*/
void computeResonCoeff() {
    for(int i=0;i<nf;i++) {
        float tmp_r = (float)(exp(-dscale*d[i]/srate));
        R2[i] = tmp_r*tmp_r;
        twoRCosTheta[i] = (float)(2.*cos(2.*M_PI*fscale*f[i]/srate)*tmp_r);
        c_i[i] = (float)(sin(2.*M_PI*fscale*f[i]/srate)*tmp_r);
    }
}

/** Compute the filter coefficients used for real-time rendering
    from the modal model parameters.
*/
void computeFilter() {
    computeResonCoeff();
    computeLocation();
}

/** Compute response through bank of modal filters.
    @param output provided output buffer.
*/
void computeSoundBuffer(float *output, float *force, int nsamples) {
    for(int k=0;k<nsamples;k++) {
        output[k] = 0;
    }
    int nf = nfUsed;

    for(int i=0;i<nf;i++) {
        float tmp_twoRCosTheta = twoRCosTheta[i];
        float tmp_R2 = R2[i];
        float tmp_a = ampR[i];
        float tmp_yt_1 = yt_1[i];
        float tmp_yt_2 = yt_2[i];
        for(int k=0;k<nsamples;k++) {
            float ynew = tmp_twoRCosTheta * tmp_yt_1 - tmp_R2 * tmp_yt_2
                + tmp_a * force[k];
            tmp_yt_2 = tmp_yt_1;
            tmp_yt_1 = ynew;
            output[k] += ynew;
        }
        yt_1[i] = tmp_yt_1;
        yt_2[i] = tmp_yt_2;
    }
}


int main(int argc, char* argv[])
{
    int bufferSize = 4096;
    float *buffer = new float[bufferSize];

    if(argc != 2) {
        printf("Usage: Bell bell4.sy\n");
        return -1;
    }

    PABLIO_Stream *outStream;
    OpenAudioStream(&outStream, srate, paFloat32, PABLIO_WRITE|PABLIO_MONO);

    readModesFile(argv[1]);

    printf("nactive_freq: %d\n",nfUsed);
    printf("n_freq: %d\n",nf);
    printf("n_points: %d\n",np);
    printf("f_scale: %f\n",fscale);
    printf("d_scale: %f\n",dscale);
    printf("a_scale: %f\n",ascale);
    printf("frequencies:\n");
    for(int i=0;i<nf;i++) {
        printf("%f\n",f[i]);
    }
    printf("dampings:\n");
    for(int i=0;i<nf;i++) {
        printf("%f\n",d[i]);
    }
    printf("amplitudes[point][freq]:\n");
    for(int p=0;p<np;p++) {
        for(int i=0;i<nf;i++) {
            printf("%f\n",a[p*nf+i]);
        }
    }

    allocateData(nf,np);
    computeFilter();

    float dur = .0002f; // 2 ms
    int nsamples = (int)(srate * dur);
    printf("nsamples: %d\n",nsamples);
    float *cosForce = new float[bufferSize];
    memset(cosForce, 0, bufferSize*sizeof(float));
    for(int i=0;i<nsamples;i++) {
        cosForce[i] = (float)(.5*(1.-cos(2*M_PI*(i+1)/(1+nsamples))));
    }

    int x;
    while(true) {
        computeSoundBuffer(buffer, cosForce, bufferSize);
        memset(cosForce, 0, nsamples*sizeof(float));
        for(int i=0;i<bufferSize;i++) {
            buffer[i] /= nf;
            // printf("%f\n",buffer[i]);
            // XXX Realy should saturate...
            if (buffer[i] > 1.0)
                buffer[i] = 1.0;
            if (buffer[i] < -1.0)
                buffer[i] = -1.0;
        }
        WriteAudioStream(outStream, buffer, bufferSize);
    }

    CloseAudioStream(outStream);

    return 0;
}
