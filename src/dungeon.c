#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "dungeon.h"
#include "../assets/dungeons.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
extern unsigned char temp_x, temp_y;
extern unsigned int temp_int;
extern unsigned char current_screen;

#pragma zpsym("temp_x");
#pragma zpsym("temp_y");
#pragma zpsym("temp_int");
#pragma zpsym("current_screen");
unsigned int nt_adr;

unsigned char mt;
#pragma bss-name(pop)

unsigned char * current_sector;
unsigned char * dungeon_layout;
unsigned char current_dungeon_index, current_sector_index;

void generate_layout(unsigned char * dungeon_layout_buffer) {
  dungeon_layout = dungeon_layout_buffer;
  temp_int = 0;
  for(temp_y = 0; temp_y < NUM_DUNGEONS; temp_y++) {
    for(temp_x = 0; temp_x < NUM_DUNGEON_LEVELS; temp_x++) {
      dungeon_layout[temp_int++] = rand8() % NUM_SECTOR_TEMPLATES;
    }
  }
}

void start_dungeon(unsigned char dungeon_index) {
  current_dungeon_index = dungeon_index;
  current_sector_index = 0;

  load_dungeon_sector(0);
}

void load_dungeon_sector(unsigned char sector_index) {
  current_sector_index = sector_index;
  current_sector = (unsigned char *) sector_metatiles[dungeon_layout[current_dungeon_index * NUM_DUNGEON_LEVELS + current_sector_index]];

  set_scroll_y(0);
  ppu_wait_nmi();
  clear_vram_buffer();

  temp_int = 0;
  nt_adr = 0x2084;
  for(temp_y = 0; temp_y < 10; temp_y++) {
    for(temp_x = 0; temp_x < 12; temp_x++) {
      mt = current_sector[temp_int++];
      one_vram_buffer(metatile_UL_tiles[mt], nt_adr);
      one_vram_buffer(metatile_UR_tiles[mt], nt_adr + 0x01);
      one_vram_buffer(metatile_DL_tiles[mt], nt_adr + 0x20);
      one_vram_buffer(metatile_DR_tiles[mt], nt_adr + 0x21);
      nt_adr += 0x02;
      if (temp_x == 6 || temp_x == 11) {
        ppu_wait_nmi();
        set_scroll_y(0);
        clear_vram_buffer();
      }
    }
    nt_adr += 0x28;
  }

  // TODO: draw level info
}
