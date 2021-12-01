#ifndef _SUBRAND_H_
#define _SUBRAND_H_

#include "farcall.h"

// Generates pseudorandom value <= upper_bound
#pragma wrapped-call (push, farcallax, bank)
#pragma code-name (push, "BANK1")
unsigned char __fastcall__ subrand8(unsigned char upper_bound);
#pragma code-name (pop)
#pragma wrapped-call (pop)

#endif
