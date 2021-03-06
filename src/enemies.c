#include "lib/subrand.h"
#include "dice.h"
#include "dungeon.h"
#include "enemies.h"
#include "entities.h"
#include "temp.h"
#include "wram.h"
#include "../assets/enemy-stats.h"

_Static_assert(Player == NUM_ENEMY_TYPES, "entity_type_enum must be aligned with number of enemies");

#pragma bss-name(push, "ZEROPAGE")

unsigned char average_difficulty, min_difficulty;

#pragma bss-name(pop)

entity_type_enum eligible_types[NUM_ENEMY_TYPES];
unsigned char num_eligible_types;

#pragma code-name ("BANK1")
#pragma rodata-name ("BANK1")

unsigned char select_enemy_type () {
  if (has_current_dungeon_yendor()) {
    temp = NUM_SECTORS;
  } else {
    temp = current_sector_index + 1;
  }
  average_difficulty = (party_level + temp) / 2;
  min_difficulty = temp / 6;
  for(i = 0; i < NUM_ENEMY_TYPES; i++) {
    if (enemy_difficulty[i] <= average_difficulty &&
        enemy_difficulty[i] >= min_difficulty) {
      eligible_types[num_eligible_types++] = i;
    }
  }
  i = subrand8(num_eligible_types - 1);
  return eligible_types[i];
}

void spawn_enemy(unsigned char entity_slot) {
  entity_type[entity_slot] = i = select_enemy_type();
  entity_moves[entity_slot] = enemy_moves[i];
  entity_speed[entity_slot] = enemy_speed[i];
  entity_attack[entity_slot] = enemy_attack[i];
  entity_status[entity_slot] = 0;
  entity_status_turns[entity_slot] = 0;
  entity_turn_counter[entity_slot] = subrand8(NORMAL_SPEED);
  temp = enemy_base_level[i];
  if (current_sector_index + 1 < temp) {
    --temp;
  } else if (current_sector_index + 1 > temp) {
    temp += (current_sector_index + 1 - temp) / 5;
  }
  if (party_level > enemy_base_level[i]) {
    temp += (party_level - enemy_base_level[i]) / 4;
  }
  if (temp * 2 > enemy_base_level[i] * 3) {
    temp = enemy_base_level[i] * 3 / 2;
  }
  if (temp > 49) temp = 49;
  entity_lv[entity_slot] = temp;

  // hp = lv d8
  entity_max_hp[entity_slot] = entity_hp[entity_slot] = roll_many_dice(temp, 8);
}
