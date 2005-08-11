/***************************************************************************
                          fh_math.hpp  -  description
                             -------------------
    begin                : Fri Feb 23 2001
    copyright            : (C) 2001 by Frank Haferkorn
    email                : Haferkorn@mail.com
 ***************************************************************************/

#ifndef __FH_MATH_HPP
#define __FH_MATH_HPP
#include <math.h>
const long      double PI=4.0*atan(1.0);
const long      double TWOPI=2.0*PI;
const long      double PIby2=0.5*PI;
const           double CDeg2Rad=PI/180;
const           double CRad2Deg=180/PI;

inline long double  sqr(double x)   {return x*x;}

inline double vAbs(     const double x,const double y,const double z=0)

	{
		return sqrt(x*x + y*y + z*z);
	}
#if 0 // #ifndef __unix__
inline float            abs( const float x)            {       return ( (x>0) ?x:-x);}
inline double           abs( const double x)           {       return ( (x>0) ?x:-x);}
inline long double      abs( const long double x)      {       return ( (x>0) ?x:-x);}
#endif

inline double Zero(const long double x,long double ZeroMax=1E-5)
{
	return  fabs(x)<ZeroMax?0.0:x;

}

inline double fmin(const double x1,const double x2){return ((x1<x2)?x1:x2);}
inline long   imin (const long  x1,const long   x2){return ((x1<x2)?x1:x2);}

inline double fmax(const double x1,const double x2){return ((x1>x2)?x1:x2);}
inline double imax(const  long  x1,const long   x2){return ((x1>x2)?x1:x2);}
inline double iabs(const  long  x1){return ((x1>0)?x1:-x1);}

inline double f2k(double f,double GroupSpeed=330)
			{               return TWOPI/GroupSpeed*f;      }
inline double deg2rad(const double deg)         {return CDeg2Rad*deg;}
inline double rad2deg(const double rad)         {return CRad2Deg*rad;}
inline double P2dB(const double pressure)       {return 20*log10(fabs(pressure)+1E-20);}
inline double dB2P(const double Level)          {return pow(10,Level/20);}
inline double I2dB(const double intensity)      {return 10*log10(intensity+1E-20);}
/*inline void sph2kar(double rho,double phi,double theta,double x,double y,double z)
{
	x=              rho     *cos(theta)     *cos(phi);
	y=              rho     *cos(theta)     *sin(phi);
	z=              rho     *sin(theta);
	return;

} */
inline void sph2kar(const double rho,const double phi,const double theta,double cartesic[])
{

	cartesic[0]=    rho     *cos(theta)     *cos(phi);
	cartesic[1]=    rho     *cos(theta)     *sin(phi);
	cartesic[2]=    rho     *sin(theta);
	return;

}
inline void kar2sph(const double *kar,double *sph)
{
	sph[0]=sqrt(kar[0]*kar[0]+kar[1]*kar[1]+kar[2]*kar[2]);
	sph[1]=atan2(kar[0],kar[1])+PIby2;
	sph[1]=asin(kar[2]/sph[0]);

	return;
}


inline double DotProd3D(const double x1[],const double x2[])
	{
		return x1[0]*x2[0]+x1[1]*x2[1]+x1[2]*x2[2];
	}


inline double    //## valid results only for theta=0 or phi=0 or phi=180°
sph2alpha(double phi, double theta)
{
	double alpha;
	if(phi<=0.5*PI||phi>1.5*PI)
		if(theta>0)     alpha=theta;
		else            alpha=fmod(theta+TWOPI,TWOPI);
	else
		alpha=PI-theta;
	return alpha;
}

inline double    //## valid results only for theta=0 or phi=0 or phi=180°
sph2rho(double phi, double theta)
{
	return phi;
}

inline void
NormalizeSpherical(const double *Source,double *Dest)
{
	if(!(Source&&Dest))
		return;
//		Exit(10,1,"source or dest musn't be NULL in Tetra::CorrectSphDir() ");

	double phi,theta;
	phi		=Source[1];
	theta	=Source[2];

	while(phi>=TWOPI)       phi-=TWOPI;
	while(phi<0.0)        	phi+=TWOPI;


	while(theta>PI) 	    theta-=TWOPI;
	while(theta<-PI)       	theta+=TWOPI;

	if(theta>PI*1.5)
		theta=theta-TWOPI;
	else if(fabs(0.5*PI-theta)<1E-3||fabs(0.5*PI+theta)<1E-3)	//theta ==90° ||theta==-90°
		phi=0;
	else if(theta>PI*0.5)
	{
		theta=fmod(PI-theta,PI);
		phi=fmod(phi+PI,TWOPI);
	}
	else if(theta<-PI*0.5)
	{
		theta= -PI-theta;
		phi=fmod(phi+PI,TWOPI);
	}



//	Dest[0]=Source[0];
	Dest[1]=phi;
	Dest[2]=theta;
}



#endif
