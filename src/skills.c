#include "charmap.h"
#include "players.h"
#include "skills.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

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
