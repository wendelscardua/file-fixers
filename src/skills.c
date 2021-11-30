#include "charmap.h"
#include "directions.h"
#include "entities.h"
#include "players.h"
#include "skills.h"
#include "temp.h"
#include "wram.h"

#pragma code-name ("BANK2")
#pragma rodata-name ("BANK2")

const skill_type skills_per_class[NumClasses - 1][6] =
  {
   { SkSlash, SkTaunt, SkShield, SkThrow, SkJoust, SkSpin },
   { SkFire, SkFreeze, SkBolt, SkConfuse, SkTele, SkThunder },
   { SkHeal, SkHaste, SkProtect, SkSlow, SkTele, SkRaise }
  };

const char skill_name[NumSkills][8] =
  {
   "........",
   "Attack  ",
   "Item    ",
   "Pass    ",
   "Bolt    ",
   "Confuse ",
   "Fire    ",
   "Freeze  ",
   "Joust   ",
   "Haste   ",
   "Heal    ",
   "Protect ",
   "Raise   ",
   "Shield  ",
   "Slash   ",
   "Slow    ",
   "Spin    ",
   "Taunt   ",
   "Throw   ",
   "Thunder ",
   "Teleport"
  };

const unsigned char skill_level[] =
  {
   0, // SkNone,
   0, // SkAttack,
   0, // SkItem,
   0, // SkPass,
   3, // SkBolt,
   3, // SkConfuse,
   1, // SkFire,
   2, // SkFreeze,
   3, // SkJoust,
   2, // SkHaste,
   1, // SkHeal,
   2, // SkProtect,
   5, // SkRaise,
   2, // SkShield,
   1, // SkSlash,
   4, // SkSlow,
   5, // SkSpin,
   2, // SkTaunt,
   3, // SkThrow,
   5, // SkThunder,
   4  // SkTele
  };

const skill_kind_t skill_kinds[] =
  {
   SkkNone, // SkNone,
   SkkMelee, // SkAttack,
   SkkNone, // SkItem,
   SkkNone, // SkPass,
   SkkForward, // SkBolt,
   SkkMelee, // SkConfuse,
   SkkForward, // SkFire,
   SkkForward, // SkFreeze,
   SkkForward, // SkJoust,
   SkkTargeted, // SkHaste,
   SkkTargeted, // SkHeal,
   SkkTargeted, // SkProtect,
   SkkRaisable, // SkRaise,
   SkkSelf, // SkShield,
   SkkMelee, // SkSlash,
   SkkTargeted, // SkSlow,
   SkkArea, // SkSpin,
   SkkArea, // SkTaunt,
   SkkMelee, // SkThrow,
   SkkEnemies, // SkThunder,
   SkkTargeted // SkTele
  };

unsigned char current_entity_skill;
unsigned char skill_target_row[MAX_SKILL_TARGETS], skill_target_col[MAX_SKILL_TARGETS];
unsigned char skill_target_entity[MAX_SKILL_TARGETS];
unsigned char skill_target_count;
direction skill_target_direction;

unsigned char have_enough_sp() {
  return player_sp[current_entity] >= 5 * skill_level[current_entity_skill];
}

void consume_sp() {
  player_sp[current_entity] -= 5 * skill_level[current_entity_skill];
}

unsigned char skill_is_targeted() {
  return skill_kinds[current_entity_skill] == SkkTargeted;
}

unsigned char set_melee_skill_target() {
  switch (skill_target_direction) {
  case Up: --skill_target_row[0]; break;
  case Down: ++skill_target_row[0]; break;
  case Left: --skill_target_col[0]; break;
  case Right: ++skill_target_col[0]; break;
  default:
    return 0;
  }

  if (skill_target_row[0] > 9 || skill_target_col[0] > 11) return 0;

  temp_x = skill_target_col[0];
  temp_y = skill_target_row[0];
  return ((skill_target_entity[0] = find_entity()) != 0xff);
}

unsigned char set_self_target() {
  skill_target_entity[0] = current_entity;
  return 1;
}

unsigned char set_forward_skill_target() {
  while(1) {
    switch (skill_target_direction) {
    case Up: --skill_target_row[0]; break;
    case Down: ++skill_target_row[0]; break;
    case Left: --skill_target_col[0]; break;
    case Right: ++skill_target_col[0]; break;
    default:
      return 0;
    }
    if (skill_target_row[0] > 9 || skill_target_col[0] > 11) return 0;
    temp_x = skill_target_col[0];
    temp_y = skill_target_row[0];
    if ((skill_target_entity[0] = find_entity()) != 0xff) return 1;
    if (collides_with_map()) return 0;
  }
}

unsigned char set_area_skill_targets() {
  return 1;
}

unsigned char skill_can_hit() {
  switch(skill_kinds[current_entity_skill]) {
  case SkkNone: return 0;
  case SkkSelf: return set_self_target();
  case SkkMelee: return set_melee_skill_target();
  case SkkForward: return set_forward_skill_target();
  case SkkTargeted: return 1;
  case SkkArea: return set_area_skill_targets();
  case SkkEnemies: return 1;
  case SkkRaisable: return 0; // TODO
  default: return 0;
  }
}
