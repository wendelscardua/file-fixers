#include "lib/neslib.h"
#include "dungeon.h"
#include "entities.h"
#include "players.h"

#pragma bss-name(push, "XRAM")
// extra RAM at $6000-$7fff

#define WRAM_VERSION 0x0006
unsigned int wram_start;
unsigned char dungeon_layout_initialized;
unsigned char wram_dungeon_layout[NUM_DUNGEONS * NUM_DUNGEON_LEVELS];
unsigned char party_initialized;
unsigned char player_name[4][5];
player_class_type player_class[4];
unsigned int player_xp[4];

unsigned char entity_lv[MAX_ENTITIES];
unsigned char entity_speed[MAX_ENTITIES];

unsigned char unrle_buffer[1024];

#pragma bss-name(pop)

#pragma code-name ("CODE")

void init_wram (void) {
  if (wram_start != WRAM_VERSION)
    memfill(&wram_start,0,0x2000);
  wram_start = WRAM_VERSION;
}
