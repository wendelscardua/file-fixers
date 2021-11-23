#include "lib/neslib.h"
#include "dice.h"
#include "dungeon.h"
#include "entities.h"
#include "players.h"
#include "skills.h"

#pragma bss-name(push, "XRAM")
// extra RAM at $6000-$7fff

#define WRAM_VERSION 0x0010
unsigned int wram_start;

unsigned char dungeon_layout_initialized;
unsigned char dungeon_layout[NUM_DUNGEONS][NUM_SECTORS];

unsigned char party_initialized;
unsigned char player_name[4][5];
player_class_type player_class[4];
unsigned int player_xp[4];
unsigned char player_max_hp[4];
skill_type player_skills[4][9];
unsigned char party_level;

entity_type_enum entity_type[MAX_ENTITIES];
unsigned char entity_lv[MAX_ENTITIES];
unsigned char entity_speed[MAX_ENTITIES];
unsigned char entity_moves[MAX_ENTITIES];
unsigned char entity_hp[MAX_ENTITIES];
dice_spec_t entity_attack[MAX_ENTITIES];

// this one doesn't need to be saved, it's here because of space
// TODO: optimize it away
unsigned char unrle_buffer[1024];

#pragma bss-name(pop)

#pragma code-name ("CODE")

void init_wram (void) {
  if (wram_start != WRAM_VERSION)
    memfill(&wram_start,0,0x2000);
  wram_start = WRAM_VERSION;
}
