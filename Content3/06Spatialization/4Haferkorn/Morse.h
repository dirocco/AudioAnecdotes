


#ifndef __MORSE_HPP
#define __MORSE_HPP
#include "Fh_math.h"
// #define EPS 1E-10
#include "Precisn.h"
#include <complex>
#if 1 // #ifdef _WIN32
using namespace std;
#endif

typedef complex<double> Complex;

Complex press(		double ka,double rbya,
					double theta,double eps,
					Complex *pplane=NULL,Complex *pscatter=NULL);

Complex pressSurface(double ka,double theta,double eps);
#endif
