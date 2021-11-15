#include "lib/neslib.h"
#include "dungeon.h"
#include "players.h"

#pragma bss-name(push, "XRAM")
// extra RAM at $6000-$7fff

#define WRAM_VERSION 0x0005
unsigned int wram_start;
unsigned char dungeon_layout_initialized;
unsigned char wram_dungeon_layout[NUM_DUNGEONS * NUM_DUNGEON_LEVELS];
unsigned char party_initialized;
unsigned char player_name[4][5];
unsigned char player_str[4];
unsigned char player_int[4];
unsigned char player_wis[4];
unsigned char player_dex[4];
unsigned char player_con[4];
unsigned int player_hp[4];
unsigned int player_max_hp[4];
unsigned int player_mp[4];
unsigned int player_max_mp[4];
unsigned int player_xp[4];
unsigned int player_lv[4];
player_class_type player_class[4];

unsigned char unrle_buffer[1024];

#pragma bss-name(pop)

#pragma code-name ("CODE")

void init_wram (void) {
  if (wram_start != WRAM_VERSION)
    memfill(&wram_start,0,0x2000);
  wram_start = WRAM_VERSION;
}
