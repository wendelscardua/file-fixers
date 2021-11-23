#ifndef _WRAM_H_
#define _WRAM_H_
#include "dungeon.h"
#include "entities.h"
#include "players.h"
#include "skills.h"

extern unsigned char unrle_buffer[];
extern unsigned char dungeon_layout_initialized;
extern unsigned char wram_dungeon_layout[NUM_DUNGEONS][NUM_SECTORS];

extern unsigned char party_initialized;
extern unsigned char player_name[4][5];
extern player_class_type player_class[];
extern unsigned int player_xp[];
extern unsigned char player_max_hp[];
extern skill_type player_skills[4][9];
extern unsigned char party_level;

extern entity_type_enum entity_type[];
extern unsigned char entity_lv[];
extern unsigned char entity_speed[];
extern unsigned char entity_moves[];
extern unsigned char entity_hp[];
extern unsigned char entity_attack[];

void init_wram (void);

#endif
