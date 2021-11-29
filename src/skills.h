#ifndef _SKILLS_H_
#define _SKILLS_H_

#include "lib/farcall.h"
#include "players.h"

typedef enum {
              // default skills
              SkNone,
              SkAttack,
              SkItem,
              SkPass,
              // learnable skills
              SkBolt, // TODO (all enemies in front?)
              SkConfuse, // TODO (confuse target?)
              SkFire, // TODO (first enemy in front?)
              SkFreeze, // TODO (reduce/zero moves?)
              SkJoust, // TODO (move forward and hit first enemy?)
              SkHaste, // TODO (increase speed - how long?)
              SkHeal, // TODO (restore hp - how much?)
              SkProtect, // TODO (add AC - how long/much?)
              SkRaise, // TODO (revive)
              SkShield, // TODO (add self AC)
              SkSlash, // TODO (strong attack)
              SkSlow, // TODO (decrease target speed)
              SkSpin, // TODO (attack around user)
              SkTaunt, // TODO (make near enemies target self)
              SkThrow, // TODO (damage and push enemy)
              SkThunder, // TODO (damage all enemies)
              SkTele, // TODO (move self to anywhere)
              NumSkills
} skill_type;

extern const skill_type skills_per_class[NumClasses - 1][6];
extern const char skill_name[NumSkills][8];

#endif
