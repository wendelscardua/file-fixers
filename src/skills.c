#include "charmap.h"
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
   { SkFire, SkBolt, SkFreeze, SkConfuse, SkTele, SkThunder },
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
   1, // SkBolt,
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

unsigned char current_entity_skill;
unsigned char skill_target_row, skill_target_col;
unsigned char skill_target_entity;

unsigned char consume_sp() {
  temp = 5 * skill_level[current_entity_skill];
  if (player_sp[current_entity] >= temp) {
    player_sp[current_entity] -= temp;
    return 1;
  } else {
    return 0;
  }
}
