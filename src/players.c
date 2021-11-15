#include "entities.h"
#include "players.h"
#include "wram.h"
#include "lib/neslib.h"
#include "charmap.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

extern unsigned char i;
#pragma zpsym("i")

void initialize_party() {
  // TODO replace with proper initialization
  for(i = 0; i < 4; i++) {
    player_xp[i] = 0;
    player_class[i] = Fighter;
    entity_lv[i] = 1;
    entity_type[i] = Player;
    entity_speed[i] = NORMAL_SPEED;
  }

  memcpy(player_name[0], "Lorem", 5);
  memcpy(player_name[1], "Ipsum", 5);
  memcpy(player_name[2], "Dolor", 5);
  memcpy(player_name[3], "Amet ", 5);

  party_level = 1;
}
