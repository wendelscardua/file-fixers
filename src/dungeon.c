#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "dungeon.h"
#include "../assets/sectors.h"
#include "../assets/sprites.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
extern unsigned char temp, temp_x, temp_y;
extern unsigned int temp_int;
extern unsigned char current_screen;

#pragma zpsym("temp");
#pragma zpsym("temp_x");
#pragma zpsym("temp_y");
#pragma zpsym("temp_int");
#pragma zpsym("current_screen");
unsigned int nt_adr;

unsigned char mt;
unsigned char sector_up_row, sector_up_column;
unsigned char sector_down_row, sector_down_column;
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
  temp = dungeon_layout[current_dungeon_index * NUM_DUNGEON_LEVELS + current_sector_index];
  /*
    temp tells which sector template to use now...
    but also tells (via bit 7) if the current sector was already completed
  */
  current_sector = (unsigned char *) sector_metatiles[temp & 0x7f];

  set_scroll_y(0);
  ppu_wait_nmi();
  clear_vram_buffer();

  multi_vram_buffer_horz(dungeon_name[current_dungeon_index], 8, NTADR_A(9, 2));
  one_vram_buffer(0x10 + (sector_index>>4), NTADR_A(26, 2));
  one_vram_buffer(0x10 + (sector_index & 0x0f), NTADR_A(27, 2));

  temp_int = 0;
  nt_adr = 0x2084;
  for(temp_y = 0; temp_y < 10; temp_y++) {
    for(temp_x = 0; temp_x < 12; temp_x++) {
      mt = current_sector[temp_int];
      switch(mt) {
      case UpMetatile:
        sector_up_row = temp_y;
        sector_up_column = temp_x;
        break;
      case DownMetatile:
        sector_down_row = temp_y;
        sector_down_column = temp_x;
        if (!(temp & 0x80)) {
          mt = LockedMetatile;
        } else if (sector_index == NUM_DUNGEON_LEVELS - 1) {
          mt = GroundMetatile;
          sector_down_row = 0xff;
          sector_down_column = 0xff;
        }
      }
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
      temp_int++;
    }
    nt_adr += 0x28;
  }
}

void dungeon_handler() {
  // TODO
}

void draw_dungeon_sprites() {
  // TODO
}