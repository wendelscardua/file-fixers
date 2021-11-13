#include "players.h"
#include "lib/neslib.h"
#include "charmap.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

extern unsigned char i;
#pragma zpsym("i")

void initialize_party() {
  // TODO replace with proper initialization
  for(i = 0; i < 4; i++) {
    player_str[i] = 1;
    player_int[i] = 1;
    player_wis[i] = 1;
    player_dex[i] = 1;
    player_con[i] = 1;
    player_hp[i] = 10;
    player_max_hp[i] = 10;
    player_mp[i] = 10;
    player_max_mp[i] = 10;
    player_xp[i] = 0;
    player_lv[i] = 1;
    player_class[i] = Fighter;
  }
  memcpy(player_name[0], "Lorem", 5);
  memcpy(player_name[1], "Ipsum", 5);
  memcpy(player_name[2], "Dolor", 5);
  memcpy(player_name[3], "Amet ", 5);
}
