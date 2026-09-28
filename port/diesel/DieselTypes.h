// Common DieselEngine definitions: return codes and random helpers.
//
// This directory is a reimplementation of the parts of Inmar Software's
// DieselEngine (2000-2001) that pikku-ukot mesoo uses, built on SDL3.

#ifndef __DIESELTYPES_H__
#define __DIESELTYPES_H__

#include <math.h>
#include "wincompat.h"

typedef int DE_RETVAL;

#define DE_OK               0
#define DE_FAILED           1
#define DE_OUTOFMEMORY      2
#define DE_FILENOTFOUND     3
#define DE_INVALIDPARAMS    4

// Random integer in [iMin, iMax]
inline int RandInt(int iMin, int iMax)
{
    return iMin + rand() % (iMax - iMin + 1);
}

// Random float in [fMin, fMax]
inline float RandFloat(float fMin, float fMax)
{
    return fMin + (fMax - fMin) * ((float)rand() / (float)RAND_MAX);
}

#endif
