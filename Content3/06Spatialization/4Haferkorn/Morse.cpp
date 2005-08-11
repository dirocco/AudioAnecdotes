/***************************************************************************
                          Morse.cpp  -  description
                             -------------------
    begin                : Fri Feb 23 2001
    copyright            : (C) 1994-2001 by Frank Haferkorn
    email                : Haferkorn@mail.com
 ***************************************************************************/


#include "Morse.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <complex>




double    plgndr(int m,int l,double x);
void    sphbes(int n,double x,double *sj,double *sy,double *sjp,double *syp);
Complex
press(double ka,double rbya, double theta,double eps,Complex *pplane,Complex *pscatter)
{
    double kr=ka*rbya;
    double    costheta=cos(theta);
    Complex I(0.0,1.0),factor;
    double    ja,jpa,jr,jpr;
    double    ya,ypa,yr,ypr;
    Complex    h2r,h2pa;
    Complex termpnew,termsnew;
    Complex termpold,termsold;
    int  m=0,im=0;
    Complex    ps(0,0),pp(0,0);
    termpnew=termsnew=Complex(0,0);
    do
    {
        termsold=termsnew;
        termpold=termpnew;

        termpnew=termsnew=Complex(0,0);


        for(im=0;im<4;im++)
        {
            switch(m%4)    // factor=pow(I,m)
            {
            case 0: factor=1  ;        break;
            case 1: factor=I;        break;
            case 2: factor=-1.0;    break;
            case 3: factor=-I ;        break;
            }

            factor*=(2*m+1)*plgndr(m,0L,costheta);

            sphbes(m,ka,&ja,&ya,&jpa,&ypa);
            h2pa    =    jpa-I*ypa;       // h2'(ka)=j'(ka)-I*y'(ka)
            sphbes(m,kr,&jr,&yr,&jpr,&ypr);
            h2r        =    jr-I*yr;                   // h2(ka)=j(ka)-I*y(ka)

            termsnew+= -factor*jpa*h2r/h2pa;        // j'(ka)/h2'(ka)*h2(kr)*I;
            termpnew+=    factor*jr;

            m++;
        }
        pp+=termpnew;
        ps+=termsnew;


    }
    while(m<1000&&(abs(termsnew)>eps||abs(termpnew)>eps));

//    cerr<<m<<tab;
    if(m>1000) std::cerr<<"more than "<<m<<" iterations !!!"<<endl;

    if(pplane!=NULL)       *pplane=pp;
    if(pscatter!=NULL)    *pscatter=ps;

    std::cerr<<m<<"_";

    return (pp+ ps);


}
