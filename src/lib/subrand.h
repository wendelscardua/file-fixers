#ifndef _SUBRAND_H_
#define _SUBRAND_H_

#include "farcall.h"

// Generates pseudorandom value <= upper_bound
#pragma wrapped-call (push, farcallax, bank)
unsigned char __fastcall__ subrand8(unsigned char upper_bound);
#pragma wrapped-call (pop)

#endif
