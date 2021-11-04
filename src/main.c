/* Based on ...
 *  example of MMC3 for cc65
 *	Doug Fraker 2019
 */

#include "lib/neslib.h"
#include "lib/nesdoug.h"
#include "lib/unrle.h"
#include "mmc3/mmc3_code.h"
#include "mmc3/mmc3_code.c"
#include "sprites.h"
#include "../assets/nametables.h"
#include "../assets/palettes.h"

#define SFX_TOGGLE 0
#define SFX_SELECT 1
#define SFX_START 2
#define SFX_THE_END 3
#define SFX_HIT 4
#define SFX_PEW 5
#define SFX_CHAOS 6

#define BG_MAIN_0 0
#define BG_MAIN_1 1
#define BG_MAIN_2 2
#define BG_MAIN_3 3
#define SPRITE_0 4
#define SPRITE_1 6
#define BG_DUNGEON_0 8
#define BG_DUNGEON_1 9
#define BG_DUNGEON_2 10
#define BG_DUNGEON_3 11

#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
unsigned char double_buffer_index;
unsigned char arg1;
unsigned char arg2;
unsigned char pad1;
unsigned char pad1_new;

unsigned char temp, i, unseeded, temp_x, temp_y;
unsigned int temp_int, temp_int_x, temp_int_y;

enum game_state {
                 Title,
                 GamePlay
} current_game_state;

#pragma bss-name(pop)
// should be in the regular 0x300 ram now

char irq_array[32];
unsigned char double_buffer[32];

#pragma bss-name(push, "XRAM")
// extra RAM at $6000-$7fff

unsigned int wram_start;

#pragma bss-name(pop)

// the fixed bank

#pragma rodata-name ("RODATA")
#pragma code-name ("CODE")

void draw_sprites (void);

void init_wram (void) {
  if (wram_start != 0xcafe)
    memfill(&wram_start,0,0x2000);
  wram_start = 0xcafe;
}

void start_game (void) {
  if (unseeded) {
    seed_rng();
    unseeded = 0;
  }

  pal_fade_to(4, 0);
  ppu_off();
  pal_bg(bg_palette);
  pal_spr(sprites_palette);

  // draw some things
  vram_adr(NTADR_A(0,0));
  unrle(title_nametable); // TODO: first screen nametable
  ppu_on_all();

  pal_fade_to(0, 4);
  current_game_state = GamePlay;
}

void go_to_title (void) {
  current_game_state = Title;

  if (irq_array[0] != 0xff) {
    while(!is_irq_done() ){}
    irq_array[0] = 0xff;
    double_buffer[0] = 0xff;
  }

  clear_vram_buffer();

  pal_fade_to(4, 0);
  ppu_off(); // screen off
  // draw some things
  vram_adr(NTADR_A(0,0));
  unrle(title_nametable);
  music_play(0);

  set_scroll_x(0);
  set_scroll_y(0);

  draw_sprites();

  set_chr_mode_2(BG_MAIN_0);
  set_chr_mode_3(BG_MAIN_1);
  set_chr_mode_4(BG_MAIN_2);
  set_chr_mode_5(BG_MAIN_3);

  pal_bg(bg_palette);
  pal_spr(sprites_palette);

  ppu_on_all(); //	turn on screen
  pal_fade_to(0, 4);
}

void main (void) {
  set_mirroring(MIRROR_HORIZONTAL);
  bank_spr(1);
  irq_array[0] = 0xff; // end of data
  set_irq_ptr(irq_array); // point to this array

  init_wram();

  ppu_off(); // screen off
  pal_bg(bg_palette); //	load the BG palette
  pal_spr(sprites_palette); // load the sprite palette

  set_chr_mode_2(BG_MAIN_0);
  set_chr_mode_3(BG_MAIN_1);
  set_chr_mode_4(BG_MAIN_2);
  set_chr_mode_5(BG_MAIN_3);
  set_chr_mode_0(SPRITE_0);
  set_chr_mode_1(SPRITE_1);

  go_to_title();

  unseeded = 1;

  set_vram_buffer();
  clear_vram_buffer();

  while (1){ // infinite loop
    ppu_wait_nmi();
    clear_vram_buffer();
    pad_poll(0);

    double_buffer_index = 0;

    switch (current_game_state) {
    case Title:
      for(i = 0; i < 16; i++) {
        rand16();
        if (get_pad_new(0) & (PAD_START | PAD_A)) {
          sfx_play(SFX_START, 0);
          start_game();
          break;
        }
        pad_poll(0);
      }
      break;
    case GamePlay:
      // TODO: gameplay
      break;
    }

    // load the irq array with values it parse
    // ! CHANGED it, double buffered so we aren't editing the same
    // array that the irq system is reading from


    double_buffer[double_buffer_index++] = 0xff; // end of data

    draw_sprites();

    // wait till the irq system is done before changing it
    // this could waste a lot of CPU time, so we do it last
    while(!is_irq_done() ){}

    // copy from double_buffer to the irq_array
    // memcpy(void *dst,void *src,unsigned int len);
    memcpy(irq_array, double_buffer, sizeof(irq_array));
  }
}

void draw_sprites (void) {
  oam_clear();
  if (current_game_state != GamePlay) return;

  // TODO: render sprites
}
