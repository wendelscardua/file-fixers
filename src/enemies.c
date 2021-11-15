#include "lib/subrand.h"
#include "entities.h"
#include "enemies.h"
#include "dungeon.h"
#include "../assets/enemy-stats.h"

_Static_assert(Player == NUM_ENEMY_TYPES, "entity_type_enum must be aligned with number of enemies");

#pragma bss-name(push, "ZEROPAGE")
extern unsigned char i, temp;

#pragma zpsym("i");
#pragma zpsym("temp");

unsigned char average_difficulty, min_difficulty;

#pragma bss-name(pop)

entity_type_enum eligible_types[NUM_ENEMY_TYPES];
unsigned char num_eligible_types;

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

unsigned char select_enemy_type () {
  average_difficulty = (party_level + current_sector_index + 1) / 2;
  min_difficulty = (current_sector_index + 1) / 6;
  for(i = 0; i < NUM_ENEMY_TYPES; i++) {
    if (enemy_difficulty[i] <= average_difficulty &&
        enemy_difficulty[i] >= min_difficulty) {
      eligible_types[num_eligible_types++] = i;
    }
  }
  i = subrand8(num_eligible_types - 1);
  return eligible_types[i];
}
