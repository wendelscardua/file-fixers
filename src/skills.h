#ifndef _SKILLS_H_
#define _SKILLS_H_

#include "lib/farcall.h"
#include "directions.h"
#include "players.h"

#define MAX_SKILL_TARGETS 8
#define LEVEL_FOR_SKILL 3

typedef enum {
              // default skills
              SkNone,
              SkAttack,
              SkItem,
              SkPass,
              // learnable skills
              SkBolt,
              SkConfuse,
              SkFire,
              SkFreeze,
              SkJoust,
              SkHaste,
              SkHeal,
              SkProtect,
              SkRaise,
              SkShield,
              SkSlash,
              SkSlow,
              SkSpin,
              SkTaunt,
              SkThrow,
              SkThunder,
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
extern unsigned char skill_target_row[MAX_SKILL_TARGETS], skill_target_col[MAX_SKILL_TARGETS];
extern unsigned char skill_target_entity[MAX_SKILL_TARGETS];
extern direction skill_target_direction;
extern unsigned char skill_target_count, skill_target_index;

#pragma code-name (push, "BANK2")

#pragma wrapped-call (push, farcallax, bank)
unsigned char have_enough_sp (void);
unsigned char skill_is_targeted (void);
unsigned char set_melee_skill_target (void);
unsigned char set_forward_skill_target (void);
unsigned char skill_can_hit (void);
#pragma wrapped-call (pop)

#pragma wrapped-call (push, farcall, bank)
void consume_sp (void);
#pragma wrapped-call (pop)

#pragma code-name (pop)

#endif
