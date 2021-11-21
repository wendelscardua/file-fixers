#include "lib/neslib.h"
#include "charmap.h"
#include "players.h"
#include "skills.h"
#include "wram.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

extern unsigned char i;
#pragma zpsym("i")

void initialize_party() {
  // TODO replace with proper initialization
  for(i = 0; i < 4; i++) {
    player_xp[i] = 0;
    player_class[i] = Fighter;
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
    // TODO: maybe per class?
    player_max_hp[i] = entity_hp[i] = 16;
  }

  memcpy(player_name[0], "Lorem", 5);
  memcpy(player_name[1], "Ipsum", 5);
  memcpy(player_name[2], "Dolor", 5);
  memcpy(player_name[3], "Amet ", 5);

  party_level = 1;
}
