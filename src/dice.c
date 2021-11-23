#include "lib/subrand.h"
#include "dice.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
unsigned char amount, sides, total;
#pragma bss-name(pop)

unsigned char roll_dice(unsigned char dice_spec) {
  amount = dice_spec & 0b111;
  sides = (dice_spec >> 3) - 1;
  total = amount;
  while(amount > 0) {
    --amount;
    total += subrand8(sides);
  }
  return total;
}

unsigned char roll_die(unsigned char sides) {
  return 1 + subrand8(sides - 1);
}
