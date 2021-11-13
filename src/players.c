#include "players.h"

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
}
