#include <math.h>
#include "libcu.h"

#ifndef log2
double DLL_PREFIX log2(double value)
{
    return (log10(value) / log10(2.0));
}
#endif
