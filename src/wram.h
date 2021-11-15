#ifndef _WRAM_H_
#define _WRAM_H_

#include "entities.h"

extern unsigned char unrle_buffer[];
extern unsigned char dungeon_layout_initialized;
extern unsigned char party_initialized;
extern unsigned char wram_dungeon_layout[];

extern entity_type_enum entity_type[];
extern unsigned char entity_lv[];
extern unsigned char entity_speed[];

void init_wram (void);

#endif
