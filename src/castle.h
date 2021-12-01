#ifndef _CASTLE_H_
#define _CASTLE_H_

#include "lib/farcall.h"

#pragma wrapped-call (push, farcallax, bank)
#pragma code-name (push, "BANK1")
void __fastcall__ init_castle_cutscene(void);
void __fastcall__ castle_handler(void);
#pragma code-name (pop)
#pragma wrapped-call (pop)
void __fastcall__ draw_castle_sprites(void);

#endif
