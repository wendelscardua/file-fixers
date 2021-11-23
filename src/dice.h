#ifndef _DICE_H_
#define _DICE_H_

#define DICE(amount,sides) ((sides << 3) | amount)

typedef struct {
  unsigned char sides : 5;
  unsigned char amount : 3;
} dice_spec_t;

_Static_assert(sizeof(dice_spec_t) == sizeof(unsigned char), "dice_spec_t must be sizeof(unsigned char)");

unsigned char roll_dice(unsigned char amount, unsigned char sides);
unsigned char roll_die(unsigned char sides);

#endif
