#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "dungeon.h"
#include "../assets/dungeons.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
unsigned char i, j, mt;
#pragma bss-name(pop)

unsigned char ** current_sector;
unsigned char ** dungeon_layout;
unsigned char current_dungeon_index, current_sector_index;

void generate_layout(unsigned char ** dungeon_layout_buffer) {
  dungeon_layout = dungeon_layout_buffer;

  for(j = 0; j < NUM_DUNGEONS; j++) {
    for(i = 0; i < NUM_DUNGEON_LEVELS; i++) {
      dungeon_layout[j][i] = rand8() % NUM_SECTOR_TEMPLATES;
    }
  }
}

void start_dungeon(unsigned char dungeon_index) {
  current_dungeon_index = dungeon_index;
  current_sector_index = 0;
}

void load_dungeon_sector(unsigned char sector_index) {
  current_sector_index = sector_index;
  current_sector = sector_metatiles[dungeon_layout[current_dungeon_index][current_sector_index]];

  for(i = 0; i < 10; i++) {
    for(j = 0; j < 12; j++) {
      mt = current_sector[i][j];
      // TODO: draw tiles
    }
  }

  // TODO: draw level info
}
