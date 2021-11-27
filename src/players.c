#include "lib/neslib.h"
#include "charmap.h"
#include "dice.h"
#include "players.h"
#include "skills.h"
#include "temp.h"
#include "wram.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

const char class_names[4][7] =
  {
   "...    ",
   "Fighter",
   "Mage   ",
   "Support"
  };

void initialize_party() {
  const dice_spec_t default_attack = { 6, 1 };

  for(i = 0; i < 4; i++) {
    player_xp[i] = 0;
    player_skills[i][0] = SkAttack;
    player_skills[i][1] = SkItem;
    player_skills[i][2] = SkPass;
    player_skills[i][3] = SkNone;
    player_skills[i][4] = SkNone;
    player_skills[i][5] = SkNone;
    player_skills[i][6] = SkNone;
    player_skills[i][7] = SkNone;
    player_skills[i][8] = SkNone;
    entity_lv[i] = 1;
    entity_type[i] = Player;
    entity_speed[i] = NORMAL_SPEED;
    entity_moves[i] = 1;
    entity_attack[i] = default_attack;
    // TODO: maybe per class?
    entity_max_hp[i] = entity_hp[i] = 16;
  }

  party_level = 1;
  party_initialized = 1;
}
