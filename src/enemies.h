#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "lib/farcall.h"

#define NUM_ENEMY_MODELS 3
#define NUM_ENEMY_TYPES 5

#pragma wrapped-call (push, farcallax, bank)
unsigned char select_enemy_type ();
void spawn_enemy (unsigned char entity_slot);
#pragma wrapped-call (pop)

#endif
