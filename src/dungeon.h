#ifndef _DUNGEON_H_
#define _DUNGEON_H_

#define NUM_SECTORS 16
#define NUM_DUNGEONS 4

typedef enum {
              NullMetatile,
              GroundMetatile,
              DownMetatile,
              UpMetatile,
              LockedMetatile
} sector_metatile;

void __fastcall__ generate_layout(void);
void __fastcall__ check_dungeon_completion(void);
unsigned char __fastcall__ has_current_dungeon_yendor(void);
unsigned char __fastcall__ dungeon_completed(unsigned char dungeon_index);
void __fastcall__ start_dungeon(unsigned char dungeon_index);
void __fastcall__ load_dungeon_sector(unsigned char sector_index);
void __fastcall__ draw_dungeon_sprites(void);
void __fastcall__ dungeon_handler(void);
void __fastcall__ unlock_sector(void);

extern unsigned char * current_sector_room_data;
extern unsigned char current_sector_index;
extern unsigned char sector_up_row, sector_up_column, sector_down_row, sector_down_column;
extern unsigned char sector_locked;
extern unsigned char current_dungeon_index;

#pragma zpsym("sector_up_row")
#pragma zpsym("sector_up_column")
#pragma zpsym("sector_down_row")
#pragma zpsym("sector_down_column")
#pragma zpsym("sector_locked")

#endif
