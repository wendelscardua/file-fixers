#define NUM_DUNGEON_LEVELS 16
#define NUM_DUNGEONS 4

void __fastcall__ generate_layout(unsigned char * dungeon_layout);
void __fastcall__ start_dungeon(unsigned char dungeon_index);
void __fastcall__ load_dungeon_sector(unsigned char sector_index);
