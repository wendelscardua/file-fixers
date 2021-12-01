#ifndef _WRAM_H_
#define _WRAM_H_
#include "dice.h"
#include "dungeon.h"
#include "entities.h"
#include "players.h"
#include "skills.h"

extern unsigned char unrle_buffer[];
extern unsigned char dungeon_layout_initialized;
extern unsigned char dungeon_layout[NUM_DUNGEONS][NUM_SECTORS];
extern unsigned char yendors, dialogs_checklist;

extern unsigned char party_initialized;
extern unsigned char player_name[4][5];
extern player_class_type player_class[];
extern unsigned int player_xp[];
extern unsigned int player_sp[];
extern unsigned int player_max_sp[4];
extern skill_type player_skills[4][9];
extern unsigned char player_items[9];
extern unsigned char party_level;

extern entity_type_enum entity_type[];
extern unsigned char entity_lv[];
extern unsigned char entity_speed[];
extern unsigned char entity_moves[];
extern unsigned int entity_hp[];
extern unsigned int entity_max_hp[];
extern dice_spec_t entity_attack[];
extern unsigned char entity_status[];
extern unsigned char entity_status_turns[];

void init_wram (void);

#endif
