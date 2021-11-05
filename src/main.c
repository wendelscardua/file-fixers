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

// Game stuff
enum game_state {
                 Title,
                 MainWindow,
                 DriversWindow,
                 AboutWindow,
                 Dungeon
} current_game_state;

unsigned char cursor_index, cursor_x, cursor_y, cursor_target_x, cursor_target_y;
signed char cursor_dx, cursor_dy;

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

void draw_main_window_sprites (void);
void draw_sprites (void);
void draw_title_sprites (void);
void go_to_title (void);
void init_wram (void);
void main_window_handler (void);
void reset_cursor(void);
void start_game (void);
void title_handler (void);
void update_cursor (void);

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

    double_buffer_index = 0;

    switch (current_game_state) {
    case Title:
      title_handler();
      break;
    case MainWindow:
      main_window_handler();
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

  switch (current_game_state) {
  case Title:
    draw_title_sprites();
    break;
  case MainWindow:
    draw_main_window_sprites();
    break;
  }
}

// ::TITLE::

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

void title_handler() {
  for(i = 0; i < 16; i++) {
    pad_poll(0);
    rand16();
    if (get_pad_new(0) & (PAD_START | PAD_A)) {
      sfx_play(SFX_START, 0);
      start_game();
      break;
    }
  }
}

void draw_title_sprites() {
}

// ::MAIN WINDOW::

const unsigned char main_window_target_x[] = { 0x20, 0x48, 0x48, 0xa8, 0x48 };
const unsigned char main_window_target_y[] = { 0x20, 0x58, 0x78, 0x78, 0x98 };
const unsigned char main_window_up[]       = {    1,    1,    1,    1,    2 };
const unsigned char main_window_down[]     = {    1,    2,    4,    4,    4 };
const unsigned char main_window_left[]     = {    1,    1,    2,    2,    4 };
const unsigned char main_window_right[]    = {    1,    3,    3,    3,    3 };

void main_window_handler() {
  rand16();
  pad_poll(0);
  pad1_new = get_pad_new(0);

  if (pad1_new) {
    signed char nudge_x = rand8() % 8 - 4;
    signed char nudge_y = rand8() % 8 - 4;
    if (pad1_new & PAD_UP) {
      cursor_index = main_window_up[cursor_index];
      cursor_target_x = main_window_target_x[cursor_index] + nudge_x;
      cursor_target_y = main_window_target_y[cursor_index] + nudge_y;
    } else if (pad1_new & PAD_DOWN) {
      cursor_index = main_window_down[cursor_index];
      cursor_target_x = main_window_target_x[cursor_index] + nudge_x;
      cursor_target_y = main_window_target_y[cursor_index] + nudge_y;
    } else if (pad1_new & PAD_LEFT) {
      cursor_index = main_window_left[cursor_index];
      cursor_target_x = main_window_target_x[cursor_index] + nudge_x;
      cursor_target_y = main_window_target_y[cursor_index] + nudge_y;
    } else if (pad1_new & PAD_RIGHT) {
      cursor_index = main_window_right[cursor_index];
      cursor_target_x = main_window_target_x[cursor_index] + nudge_x;
      cursor_target_y = main_window_target_y[cursor_index] + nudge_y;
    }

    if (cursor_target_x != cursor_x || cursor_target_y != cursor_y) {
      cursor_dx = cursor_target_x - cursor_x;
      cursor_dy = cursor_target_y - cursor_y;
      while (cursor_dx > 4 || cursor_dy > 4 || cursor_dx < -4 || cursor_dy < -4) {
        if (cursor_dx > 1 || cursor_dx < -1) cursor_dx >>= 1;
        if (cursor_dy > 1 || cursor_dy < -1) cursor_dy >>= 1;
      }
    }
  }

  update_cursor();
}

void draw_main_window_sprites() {
  oam_meta_spr(cursor_x, cursor_y, arrow_cursor_sprite);
}

// ::CURSOR::

void reset_cursor () {
  cursor_index = 0;
  cursor_x = cursor_target_x = 0x80;
  cursor_y = cursor_target_y = 0x20;
  cursor_dx = cursor_dy = 0;
}

void update_cursor () {
  if (cursor_dx != 0) {
    cursor_x += cursor_dx;
    if ((cursor_dx > 0) == (cursor_x >= cursor_target_x)) {
      cursor_x = cursor_target_x;
      cursor_dx = 0;
    }
  }

  if (cursor_dy != 0) {
    cursor_y += cursor_dy;
    if ((cursor_dy > 0) == (cursor_y >= cursor_target_y)) {
      cursor_y = cursor_target_y;
      cursor_dy = 0;
    }
  }
}

// ::ETC::

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
  unrle(main_window_nametable);
  ppu_on_all();

  pal_fade_to(0, 4);
  current_game_state = MainWindow;

  reset_cursor();
}

void init_wram (void) {
  if (wram_start != 0xcafe)
    memfill(&wram_start,0,0x2000);
  wram_start = 0xcafe;
}
