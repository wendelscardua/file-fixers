#include "lib/neslib.h"
#include "lib/subrand.h"
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

// indexed by class-1 and current level-1
const unsigned char moves_per_class_and_level[3][30] =
  {
   // Fighter
   {
    2, // 1
    2, // 2
    2, // 3
    3, // 4
    3, // 5
    3, // 6
    3, // 7
    4, // 8
    4, // 9
    4, // 10
    4, // 11
    4, // 12
    4, // 13
    4, // 14
    5, // 15
    5, // 16
    5, // 17
    5, // 18
    5, // 19
    5, // 20
    5, // 21
    5, // 22
    5, // 23
    5, // 24
    6, // 25
    6, // 26
    6, // 27
    6, // 28
    6, // 29
    7  // 30
   },
   // Mage
   {
    1, // 1
    1, // 2
    2, // 3
    2, // 4
    2, // 5
    2, // 6
    3, // 7
    3, // 8
    3, // 9
    3, // 10
    3, // 11
    4, // 12
    4, // 13
    4, // 14
    4, // 15
    4, // 16
    4, // 17
    5, // 18
    5, // 19
    5, // 20
    5, // 21
    5, // 22
    5, // 23
    5, // 24
    6, // 25
    6, // 26
    6, // 27
    6, // 28
    6, // 29
    7  // 30
   },
   // Support
   {
    1, // 1
    1, // 2
    1, // 3
    1, // 4
    2, // 5
    2, // 6
    2, // 7
    2, // 8
    2, // 9
    2, // 10
    2, // 11
    2, // 12
    3, // 13
    3, // 14
    3, // 15
    3, // 16
    3, // 17
    3, // 18
    3, // 19
    3, // 20
    3, // 21
    3, // 22
    3, // 23
    3, // 24
    4, // 25
    4, // 26
    4, // 27
    4, // 28
    4, // 29
    5  // 30
   }
  };


void initialize_party() {
  const dice_spec_t default_attack = { 6, 1 };

  for(i = 0; i < 4; i++) {
    player_xp[i] = 0;
    for(temp = 0; temp < 9; temp++) {
      player_skills[i][temp] = SkNone;
      player_items[temp] = 0;
    }
    player_skills[i][0] = SkAttack;
    player_skills[i][1] = SkItem;
    player_skills[i][2] = SkPass;
    entity_lv[i] = 1;
    entity_type[i] = Player;
    entity_speed[i] = NORMAL_SPEED;
    entity_moves[i] = moves_per_class_and_level[player_class[i] - 1][0];
    entity_attack[i] = default_attack;
    // TODO: maybe per class?
    entity_max_hp[i] = entity_hp[i] = 16;

    // TODO: maybe add wiz attribute and stuff
    switch(player_class[i]) {
    case Fighter:
      player_max_sp[i] = player_sp[i] = 1 + 2 + subrand8(2);
      break;
    case Mage:
      player_max_sp[i] = player_sp[i] = 1 + 5 + subrand8(2);
      break;
    case Support:
      player_max_sp[i] = player_sp[i] = 1 + 2 + subrand8(2);
      break;
    default:
      // TODO error?
      break;
    }
  }

  party_level = 1;
  party_initialized = 1;
}
