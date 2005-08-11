/***************************************************************************
                          Rayleigh.cpp  -  description
                             -------------------
    begin                : Fri Feb 23 2001
    email                Haferkorn@mail.com
 ***************************************************************************/

#if defined(__unix__) || defined(__APPLE_CC__)
#include <unistd.h>
#else
#include <io.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <strstream>

#include <math.h>
#include "Fh_math.h"

#include "Morse.h"
const double  SoundSpeed=330; // Meter per second

const double TauTotzeitMaximum=1.0/SoundSpeed;
const double eps=EPS*1000;

;
double Angle(double HeadRadius,double phi,double theta,double *xear)
{
    //phi    =    Azimut
    //theta    =    Elevation
        double x[3];
        x[0]=HeadRadius*cos(PI/180*theta)*cos(PI/180*(phi+90));
        x[1]=HeadRadius*cos(PI/180*theta)*sin(PI/180*(phi+90));
        x[2]=HeadRadius*sin(PI/180*theta);
        // cos(alpha) = R*R_ear/(|R|*|R_ear|)

        return acos((x[0]*xear[0]+x[1]*xear[1]+x[2]*xear[2])            /
                sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2])                    /
                sqrt(xear[0]*xear[0]+xear[1]*xear[1]+xear[2]*xear[2])
              );


}
#define tab '\t'

// all units in metric (SI-) units
int main(int argc,char *argv[])
{
//unused::    char     frqname[256];
    char *FrqName;
    double    PhiBegin,PhiEnd,PhiOffset;
    double    EarAzimut,EarElevation,HeadRadius,rbya,Drbya;

//    GET Parameters
    if(argc!=1&&(strcmp(argv[1],"--help")==0) )
    {
        // Print USAGE information on arg --help
        cerr<<"USAGE:\n";
        cerr<<"rayleigh"
            <<endl<<tab<<"InFile.frq"  <<tab<<"file containing the lines of frequencies calculate"
            <<endl<<tab<<"PhiBegin"    <<tab<<"horizontal angle at which calculation starts"
            <<endl<<tab<<"PhiEnd"      <<tab<<tab<<"horizontal angle at which calculation ends"
            <<endl<<tab<<"PhiOffset"   <<tab<<"Offset of horizontal angle"
            <<endl<<tab<<"EarAzimut"   <<tab<<"azimut of the ear"
            <<endl<<tab<<"EarElevation"<<tab<<"elevation of ear"
            <<endl<<tab<<"HeadRadius  "<<tab<<"head radius"
            <<endl<<tab<<"rByr0     "  <<tab<<"calcul÷ated sperical distance relative to head"
            <<endl<<tab<<"deltarByr0"  <<tab<<"stepping in radial direction. WARNING--instable"
            <<endl;
            ;
      return 10;

    }
    else // we can opereate if args are valid
    {
        FrqName=argv[1];                    // Arg1 = Frequency
        if(argc>=4)
        {
            PhiBegin=atof(argv[2]);            // Arg2+3 Horizontal "from" and "to" Angles 
            PhiEnd    =atof(argv[3]);
        }
        else                    // calc complete circle
        {
            PhiBegin=0;                        // preset from angle =0 
            PhiEnd=360;                        // preset to angle = 360
        }

        if(argc>=5)    PhiOffset=atof(argv[4]);    // horizontal angleoffset preset is 15
        else        PhiOffset=15;

        // ears at the left and right side by default
        if(argc>=6)    EarAzimut=atof(argv[5]);        
        else        EarAzimut=90;
        
        if(argc>=7)    EarElevation=atof(argv[6]);
        else        EarElevation=0;

        // set the default head radius to 0.01m is 10cm , the bowling ball
        if(argc>=8) HeadRadius=atof(argv[7]);
        else        HeadRadius=0.01;

        // the following is used if calculating outside the surface
        // the calculation away the surface is numerically very, very unstable 
        //     as the precision of the underlying algorithms is limited
        if(argc>=9) rbya=atof(argv[8]);
        else        rbya=1.0;

        if(argc>=10)Drbya=atof(argv[9]);
        else        Drbya=0.5;


    }


    //     Frequencies to be calculated stay in a file like *.frq
    // at maximum 400 frequencies allowed
    double F[400],fsoll;
    char        lineFrq[1024];
    int ifrq=0;
  if(argc>1)
  {
    // read frequencys from "FrqName".frq
      if(access(FrqName,06)!=0)
      {
          cerr<<"cant access "<<FrqName<<endl;
          return 10;
      }

      ifstream *Ffrq=new ifstream(FrqName);
      for(ifrq=0;(!Ffrq->eof())&&ifrq<400;ifrq++)
      {
          Ffrq->getline(lineFrq,1020);
          istrstream Infrq(lineFrq);
          fsoll=0;
          Infrq>>fsoll;
          F[ifrq]=fsoll;
          if(fsoll==0) break;
    }
       delete Ffrq;
    }
  else // no frq file given, 
      //  calc all multiples of 200 Hz
    for(ifrq=1;ifrq<100;ifrq++)
      {
          fsoll=ifrq*200.0;
          F[ifrq-1]=fsoll;
    }
    int nFrq=ifrq-1;


// init ear variables
    double     phi,theta;
    double     frequency,angleR,angleL;
    double xearL[3],xearR[3];


//    calc EarKoordinates on carthesic coordinates
    xearL[0]    =HeadRadius*cos(PI/180*EarElevation)*cos(PI/180*(EarAzimut+90));
    xearL[1]    =HeadRadius*cos(PI/180*EarElevation)*sin(PI/180*(EarAzimut+90));
    xearL[2]    =HeadRadius*sin(PI/180*EarElevation);
    xearR[0]=  -xearL[0];
    xearR[1]=    xearL[1];
    xearR[2]=     xearL[2];
//    LOOP through frequencys

    long double WellenVektor0,WellenVektor1;
    long double dW;
    long double dPhaseL,dPhaseR;
    long double tauL,tauR;
    Complex paL0,paL1, paR0,paR1;
    double ps=-1;
    theta=0;
    long double phase,phase1;
    double Inclination,ITD,KuhnITD,KuhnPiL,KuhnPiH;

    cout<<   "#"
        <<    "phi"        <<tab
        <<    "Theta"      <<tab
        <<    "f[kHz]"     <<tab
        <<    "ka"         <<tab
        <<    "r/a"        <<tab
        <<    "L_left"     <<tab
        <<    "dL"         <<tab
        <<    "p_left"     <<tab
        <<    "ps_left"    <<tab
        <<    "arg(A)"     <<tab
        <<    "T_l_ns"     <<tab
        <<    "ITD_ns"     <<tab
        <<    "ITDKuhn"    <<tab
        <<    "dT_ns"      <<tab
        <<    "PiL"        <<tab
        <<    "PiH"        <<tab
        <<    "T_l[ms]"    <<tab
        <<    "ITD[ms]"    <<tab
        <<    "ITDKuhn"    <<tab
        <<    "dT_[ms]"    <<tab

        <<endl;


    for(double RbyA=1.0;RbyA<=rbya;RbyA+=Drbya)
    //    Loop thru surfaces above the head
    for(frequency=F[ifrq=0]; ifrq<nFrq; frequency=F[++ifrq])
    //    Loop thru frequencies
    {
            cerr<<    setprecision(3)
        //        <<    phi
                <<     frequency
                <<    endl;

            for(phi=PhiBegin; phi<=PhiEnd; phi+=PhiOffset)
            //    Loop thru angles
            {
                angleR=Angle(HeadRadius,phi,theta,xearR);
                angleL=Angle(HeadRadius,phi,theta,xearL);


                WellenVektor0=TWOPI*frequency/SoundSpeed;
                paR0=    press(WellenVektor0*HeadRadius,RbyA,angleR,eps);
                paL0=    press(WellenVektor0*HeadRadius,RbyA,angleL,eps);

                dW=TWOPI*frequency*1E-2;
                WellenVektor1=(TWOPI*frequency+dW)/SoundSpeed;

                paR1=    press(WellenVektor1*HeadRadius,RbyA,angleR,eps);
                paL1=    press(WellenVektor1*HeadRadius,RbyA,angleL,eps);

                cerr<<endl;

             //    correct possible phase jump because of arg() is fmod(phase,TWOPI)

                phase=arg(paL0);
                phase1=arg(paL1);
                dPhaseL=arg(paL0/paL1); // =arg(paL1)-arg(paL0);

                dPhaseR=arg(paR0/paR1);//=arg(paR1)-arg(paR0);

             //    detect phase jump from 2PI --> 0;

                if        (dPhaseL/dW<    -TauTotzeitMaximum)    dPhaseL+=PI;
                else if    (dPhaseL/dW>    TauTotzeitMaximum)     dPhaseL-=PI;

                if        (dPhaseR/dW<    -TauTotzeitMaximum)    dPhaseR+=PI;
                else if    (dPhaseR/dW>    TauTotzeitMaximum)    dPhaseR-=PI;

             // Gruppenlaufzeit
             //    Group-runtime tau:=dPhase/dOmega

                tauL=dPhaseL/dW;
                tauR=dPhaseR/dW;

             //    Comparison with G.F.Kuhn's
             //    "Localization in the azimutal plane"
             //    in J.A.S.A, Vol 62 ,No.1 , July 1977

             //    get Theta_inc
                if(angleR<angleL)    Inclination    =PIby2-angleR;
                else                Inclination    =PIby2-angleL;

                cerr<<Zero(rad2deg(Inclination))<<tab;
                ITD            =(tauL-tauR);
                KuhnPiL    =    ITD/( HeadRadius/SoundSpeed*sin(Inclination) );

                KuhnITD    =     HeadRadius/SoundSpeed*(Inclination+sin(Inclination));
                KuhnPiH    =    fabs(ITD)/KuhnITD;

                if(KuhnITD<1E-8)    KuhnPiL=KuhnPiH=0;

                cout<<    setprecision(3)

                    <<    Zero((phi==360)?360:fmod(phi,360))     <<tab
                    <<    theta                            <<tab
                    <<    frequency/1000                   <<tab

                    <<    WellenVektor0*HeadRadius         <<tab

                    <<    RbyA                             <<tab

                    <<    P2dB(abs(paL0))                  <<tab
                    <<    P2dB(abs(paL0)/abs(paR0))        <<tab

                    <<    abs(paL0)                        <<tab
                    <<    abs(ps)                          <<tab
                    <<    arg(paL0)                        <<tab


                    <<    setprecision(0)
                    <<    Zero(tauL*1E9)                   <<tab
                    <<    Zero(ITD *1E9)                   <<tab
                    <<    Zero(KuhnITD*1E9)                <<tab
                    <<    Zero((ITD-KuhnITD)*1E9)          <<tab

                    <<    setprecision(4)
                    <<    KuhnPiL                          <<tab
                    <<    KuhnPiH                          <<tab
                    ;
                cout<<    setprecision(4)
                    <<    Zero(tauL*1E3)                    <<tab
                    <<    Zero(ITD *1E3)                    <<tab
                    <<    Zero(KuhnITD*1E3)                 <<tab
                    <<    Zero((ITD-KuhnITD)*1E3)           <<tab

                    <<endl;

                cout<<"";
            }

            cout<<endl;

        }
    return 0;
}
