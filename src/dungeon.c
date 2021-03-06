#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "lib/unrle.h"
#include "dice.h"
#include "dungeon.h"
#include "entities.h"
#include "temp.h"
#include "wram.h"
#include "../assets/sectors.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")

unsigned int nt_adr;

unsigned char mt;
unsigned char sector_up_row, sector_up_column;
unsigned char sector_down_row, sector_down_column;
unsigned char sector_locked;
#pragma bss-name(pop)

unsigned char * current_sector;
unsigned char * current_sector_room_data;
unsigned char current_dungeon_index, current_sector_index;


void generate_layout() {
  _Static_assert(NUM_SECTOR_TEMPLATES == 8 || NUM_SECTOR_TEMPLATES == 16 || NUM_SECTOR_TEMPLATES == 32 || NUM_SECTOR_TEMPLATES == 64 || NUM_SECTOR_TEMPLATES == 128);
  for(temp_y = 0; temp_y < NUM_DUNGEONS; temp_y++) {
    for(temp_x = 0; temp_x < NUM_SECTORS; temp_x++) {
      do {
        dungeon_layout[temp_y][temp_x] = temp = rand8() & (NUM_SECTOR_TEMPLATES-1);
      } while(temp_x > 0 && temp == dungeon_layout[temp_y][temp_x - 1]);
    }
  }
}

void check_dungeon_completion() {
  if (yendors & (1 << current_dungeon_index)) {
    yendors |= (16 << current_dungeon_index);
  }
}

unsigned char dungeon_completed(unsigned char dungeon_index) {
  return yendors & (16 << dungeon_index);
}

unsigned char has_current_dungeon_yendor() {
  return yendors & (1 << current_dungeon_index);
}

void start_dungeon(unsigned char dungeon_index) {
  current_dungeon_index = dungeon_index;
  current_sector_index = 0;

  load_dungeon_sector(0);
  init_entities(sector_up_row, sector_up_column);
}

void load_dungeon_sector(unsigned char sector_index) {
  current_sector_index = sector_index;
  temp = dungeon_layout[current_dungeon_index][current_sector_index];
  /*
    temp tells which sector template to use now...
    but also tells (via bit 7) if the current sector was already completed
  */
  sector_locked = ((temp & 0x80) == 0);
  temp &= 0x7f;
  set_unrle_buffer(unrle_buffer);
  unrle_to_buffer((unsigned char *) sector_metatiles[temp]);
  current_sector = unrle_buffer;
  current_sector_room_data = (unsigned char *) sector_rooms[temp];

  set_scroll_y(0);
  ppu_wait_nmi();
  clear_vram_buffer();

  multi_vram_buffer_horz(dungeon_name[current_dungeon_index], 8, NTADR_A(9, 2));

  // display sector_index as hexadecimal
  one_vram_buffer(0x10 + (sector_index>>4), NTADR_A(26, 2)); // works for sector < a0
  i = (sector_index & 0x0f) + 0x10;
  if (i > 0x19) i += 0x07;
  one_vram_buffer(i, NTADR_A(27, 2));

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
        if (sector_index == NUM_SECTORS - 1) {
          mt = GroundMetatile;
          sector_down_row = 0xff;
          sector_down_column = 0xff;
        } else if (sector_locked) {
          mt = LockedMetatile;
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
  entity_handler();
}

void draw_dungeon_sprites() {
  draw_entities();
}

void unlock_sector() {
  if (!sector_locked) return;

  sector_locked = 0;

  if (current_sector_index == NUM_SECTORS - 1) {
    yendors |= (1 << current_dungeon_index);
    return;
  }

  dungeon_layout[current_dungeon_index][current_sector_index] |= 0x80;

  nt_adr = 0x2084 + sector_down_row * 0x40 + sector_down_column * 0x02;

  one_vram_buffer(metatile_UL_tiles[DownMetatile], nt_adr);
  one_vram_buffer(metatile_UR_tiles[DownMetatile], nt_adr + 0x01);
  one_vram_buffer(metatile_DL_tiles[DownMetatile], nt_adr + 0x20);
  one_vram_buffer(metatile_DR_tiles[DownMetatile], nt_adr + 0x21);
}
