#define NUM_DUNGEON_LEVELS 16
#define NUM_DUNGEONS 4

typedef enum {
              NullMetatile,
              GroundMetatile,
              DownMetatile,
              UpMetatile,
              LockedMetatile
} sector_metatile;

void __fastcall__ generate_layout(unsigned char * dungeon_layout);
void __fastcall__ start_dungeon(unsigned char dungeon_index);
void __fastcall__ load_dungeon_sector(unsigned char sector_index);
void __fastcall__ draw_dungeon_sprites(void);
void __fastcall__ dungeon_handler(void);

extern unsigned char * current_sector_room_data;
extern unsigned char current_sector_index;
