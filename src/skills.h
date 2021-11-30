#ifndef _SKILLS_H_
#define _SKILLS_H_

#include "lib/farcall.h"
#include "directions.h"
#include "players.h"

#define LEVEL_FOR_SKILL 3

typedef enum {
              // default skills
              SkNone,
              SkAttack,
              SkItem,
              SkPass,
              // learnable skills
              SkBolt, // TODO (all enemies in front?)
              SkConfuse,
              SkFire, // TODO (first enemy in front?)
              SkFreeze, // TODO (reduce/zero moves?)
              SkJoust, // TODO (move forward and hit first enemy?)
              SkHaste,
              SkHeal,
              SkProtect,
              SkRaise, // TODO (revive)
              SkShield,
              SkSlash,
              SkSlow,
              SkSpin, // TODO (attack around user)
              SkTaunt, // TODO (make near enemies target self)
              SkThrow,
              SkThunder, // TODO (damage all enemies)
              SkTele,
              NumSkills
} skill_type;

typedef enum {
              SkkNone,
              SkkSelf,
              SkkMelee,
              SkkForward,
              SkkTargeted,
              SkkArea,
              SkkEnemies,
              SkkRaisable
} skill_kind_t;

extern const skill_type skills_per_class[NumClasses - 1][6];
extern const char skill_name[NumSkills][8];

extern unsigned char current_entity_skill;
extern unsigned char skill_target_row, skill_target_col;
extern unsigned char skill_target_entity;
extern direction skill_target_direction;

#pragma wrapped-call (push, farcallax, bank)
unsigned char have_enough_sp (void);
unsigned char skill_is_targeted (void);
unsigned char set_melee_skill_target (void);
unsigned char skill_can_hit (void);
#pragma wrapped-call (pop)

#pragma wrapped-call (push, farcall, bank)
void consume_sp (void);
#pragma wrapped-call (pop)

#endif
