public double[] f;
public double[] d;
public double[][] a;

public double fscale;
public double dscale;
public double ascale;

public interface AudioForce {
    public void getForce(double [] output, int nsamples);
}

public abstract class SonicObject
    extends Thread implements AudioForce

public void run() {
    float [] y = new float[bufferSize];
    RTPlay pb = new RTPlay(bufferSizeJavaSound,srate,16,1,true);
    while(true) {
        this.getForce(y,bufferSize);
        // Unimportant code omitted ...
        pb.write(y);
    }
}

public void getForce(float [] output, int nsamples) {
    audioForce.getForce(scratchBuf, nsamples);
    computeSoundBuffer(output, scratchBuf, nsamples);
}

private void computeSoundBuffer(float[] output, float[] force, int nsamples) {

    for(int k=0;k<nsamples;k++) {
        output[k] = 0;
    }

    int nf = modalModel.nfUsed;

    for(int i=0;i<nf;i++) {

        float tmp_twoRCosTheta = twoRCosTheta[i];
        float tmp_R2 = R2[i];
        float tmp_a = ampR[i];
        float tmp_yt_1 = yt_1[i];
        float tmp_yt_2 = yt_2[i];

        for(int k=0;k<nsamples;k++) {
            float ynew = tmp_twoRCosTheta * tmp_yt_1 -
                  tmp_R2 * tmp_yt_2 + tmp_a * force[k];
            tmp_yt_2 = tmp_yt_1;
            tmp_yt_1 = ynew;
            output[k] += ynew;
        }

        yt_1[i] = tmp_yt_1;
        yt_2[i] = tmp_yt_2;

    }
}

/* "Its main() method contains the lines" */
// sob1 = new SonicObject(new ModalModel(args[0]),
// srate,bufferSize,bufferSizeJavaSound);
// af1 = new BangForce(srate);
// sob1.setAudioForce(af1);
// sob1.start();
// new HelperThread().start();
// new DemoBellStrike (new java.awt.Frame (), true).show ();

while(true) {
    sleep(t_control);
    af1.hit();
}

af1.setDuration(x);

protected RTAudioIn pai;
    public void getForce(double [] output, int nsamples) {
        pai.read(output,nsamples);
    }
}
