#ifndef _WRAM_H_
#define _WRAM_H_
#include "entities.h"
#include "players.h"

extern unsigned char unrle_buffer[];
extern unsigned char dungeon_layout_initialized;
extern unsigned char party_initialized;
extern unsigned char wram_dungeon_layout[];

extern unsigned char party_level;
extern unsigned char player_xp[];
extern player_class_type player_class[];
extern unsigned char player_name[4][5];

extern entity_type_enum entity_type[];
extern unsigned char entity_lv[];
extern unsigned char entity_speed[];
extern unsigned char entity_moves[];

void init_wram (void);

#endif
