/* Based on ...
 *  example of MMC3 for cc65
 *	Doug Fraker 2019
 */

#include "lib/neslib.h"
#include "lib/nesdoug.h"
#include "lib/unrle.h"
#include "mmc3/mmc3_code.h"
#include "mmc3/mmc3_code.c"
#include "nametable_loader.h"
#include "dungeon.h"
#include "players.h"
#include "../assets/sprites.h"
#include "../assets/nametables.h"
#include "../assets/palettes.h"

#define FP(integer,fraction) (((integer)<<8)|((fraction)>>0))
#define INT(unsigned_fixed_point) ((unsigned_fixed_point>>8)&0xff)

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
#define SPRITE_PLAYERS_0 12
#define SPRITE_PLAYERS_1 14

// Configs
#define CLICK_DELAY 0x10
#define MAX_CURSOR_DX FP(8, 0)
#define MAX_CURSOR_DY FP(4, 0)
#define MIN_CURSOR_DX FP(0, 8)
#define MIN_CURSOR_DY FP(0, 8)


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

unsigned char cursor_index, cursor_counter;
unsigned int cursor_x, cursor_y, cursor_target_x, cursor_target_y;
signed int cursor_dx, cursor_dy;

enum cursor_state {
                   Default,
                   Clicking,
                   Loading
} current_cursor_state;

unsigned char current_screen;

#pragma bss-name(pop)
// should be in the regular 0x300 ram now

char irq_array[32];
unsigned char double_buffer[32];

#pragma bss-name(push, "XRAM")
// extra RAM at $6000-$7fff

#define WRAM_VERSION 0x0002
unsigned int wram_start;
unsigned char dungeon_layout_initialized;
unsigned char wram_dungeon_layout[NUM_DUNGEONS * NUM_DUNGEON_LEVELS];
unsigned char party_initialized;
unsigned char player_name[4][5];
unsigned char player_str[4];
unsigned char player_int[4];
unsigned char player_wis[4];
unsigned char player_dex[4];
unsigned char player_con[4];
unsigned int player_hp[4];
unsigned int player_max_hp[4];
unsigned int player_mp[4];
unsigned int player_max_mp[4];
unsigned int player_xp[4];
unsigned int player_lv[4];
player_class_type player_class[4];

unsigned char unrle_buffer[1024];

#pragma bss-name(pop)

// the fixed bank

#pragma rodata-name ("RODATA")
#pragma code-name ("CODE")

void draw_cursor (void);
void draw_drivers_window_sprites (void);
void draw_main_window_sprites (void);
void draw_sprites (void);
void draw_title_sprites (void);
void drivers_window_default_cursor_handler (void);
void drivers_window_loading_handler (void);
void drivers_window_handler (void);
void flip_screen (void);
void go_to_title (void);
void init_wram (void);
void main_window_default_cursor_handler (void);
void main_window_loading_handler (void);
void main_window_handler (void);
void reset_cursor (void);
void set_cursor_speed (void);
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
    case DriversWindow:
      drivers_window_handler();
      break;
    case Dungeon:
      dungeon_handler();
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
  case DriversWindow:
    draw_drivers_window_sprites();
    break;
  case Dungeon:
    draw_dungeon_sprites();
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
  vram_unrle(title_nametable);
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

void main_window_handler() {
  rand16();
  pad_poll(0);
  pad1_new = get_pad_new(0);

  switch(current_cursor_state) {
  case Default:
    main_window_default_cursor_handler();
    break;
  case Loading:
    main_window_loading_handler();
    break;
  }

  update_cursor();
}

void draw_main_window_sprites() {
  draw_cursor();
}

const unsigned char main_window_target_x[] = { 0x20, 0x48, 0x48, 0xa8, 0x48 };
const unsigned char main_window_target_y[] = { 0x20, 0x58, 0x78, 0x78, 0x98 };
const unsigned char main_window_up[]       = {    1,    1,    1,    1,    2 };
const unsigned char main_window_down[]     = {    1,    2,    4,    4,    4 };
const unsigned char main_window_left[]     = {    1,    1,    2,    2,    4 };
const unsigned char main_window_right[]    = {    1,    3,    3,    3,    3 };

void main_window_default_cursor_handler() {
  if (pad1_new) {
    signed char nudge_x = rand8() % 8 - 4;
    signed char nudge_y = rand8() % 8 - 4;
    if (pad1_new & PAD_UP) {
      cursor_index = main_window_up[cursor_index];
      cursor_target_x = FP(main_window_target_x[cursor_index] + nudge_x, 0);
      cursor_target_y = FP(main_window_target_y[cursor_index] + nudge_y, 0);
    } else if (pad1_new & PAD_DOWN) {
      cursor_index = main_window_down[cursor_index];
      cursor_target_x = FP(main_window_target_x[cursor_index] + nudge_x, 0);
      cursor_target_y = FP(main_window_target_y[cursor_index] + nudge_y, 0);
    } else if (pad1_new & PAD_LEFT) {
      cursor_index = main_window_left[cursor_index];
      cursor_target_x = FP(main_window_target_x[cursor_index] + nudge_x, 0);
      cursor_target_y = FP(main_window_target_y[cursor_index] + nudge_y, 0);
    } else if (pad1_new & PAD_RIGHT) {
      cursor_index = main_window_right[cursor_index];
      cursor_target_x = FP(main_window_target_x[cursor_index] + nudge_x, 0);
      cursor_target_y = FP(main_window_target_y[cursor_index] + nudge_y, 0);
    }

    if (cursor_target_x == cursor_x && cursor_target_y == cursor_y) {
      if (pad1_new & PAD_A) {
        current_cursor_state = Clicking;
        cursor_counter = CLICK_DELAY;
      }
    } else {
      set_cursor_speed();
    }
  }
}

void main_window_loading_handler () {
  // started loading
  switch(cursor_index) {
  case 0:
    current_cursor_state = Default;
    break;
  case 1: // Castle.exe
    current_cursor_state = Default;
    // TODO: go to castle
    break;
  case 2: // Drivers
    if (cursor_counter == 0) {
      set_unrle_buffer(unrle_buffer);
      unrle_to_buffer(drivers_window_nametable);

      set_nametable_loader_buffer(unrle_buffer);
      if (current_screen == 0) {
        start_nametable_loader(NTADR_C(0, 0));
      } else {
        start_nametable_loader(NTADR_A(0, 0));
      }
    } else {
      if (!yield_nametable_loader()) {
        current_cursor_state = Default;
        cursor_index = 0;
        cursor_target_x = FP(0xbc, 0);
        cursor_target_y = FP(0x44, 0);
        set_cursor_speed();
        current_game_state = DriversWindow;
        flip_screen();
      }
    }
    break;
  case 3: // About.txt
    // TODO
    current_cursor_state = Default;
    break;
  case 4: // Config.sys
    // TODO
    current_cursor_state = Default;
    break;
  }
}

// ::DRIVERS WINDOW::

void drivers_window_handler() {
  rand16();
  pad_poll(0);
  pad1_new = get_pad_new(0);

  switch(current_cursor_state) {
  case Default:
    drivers_window_default_cursor_handler();
    break;
  case Loading:
    drivers_window_loading_handler();
    break;
  }

  update_cursor();
}

void draw_drivers_window_sprites() {
  draw_cursor();
}

const unsigned char drivers_window_target_x[] = { 0xbc, 0x38, 0x78, 0x38, 0x78 };
const unsigned char drivers_window_target_y[] = { 0x44, 0x58, 0x58, 0x88, 0x88 };
const unsigned char drivers_window_up[]       = {    2,    0,    0,    1,    2 };
const unsigned char drivers_window_down[]     = {    2,    3,    4,    3,    4 };
const unsigned char drivers_window_left[]     = {    2,    1,    1,    3,    3 };
const unsigned char drivers_window_right[]    = {    2,    2,    2,    4,    4 };

void drivers_window_default_cursor_handler() {
  if (pad1_new) {
    signed char nudge_x = cursor_index == 0 ? 0 : rand8() % 8 - 4;
    signed char nudge_y = cursor_index == 0 ? 0 : rand8() % 8 - 4;
    if (pad1_new & PAD_UP) {
      cursor_index = drivers_window_up[cursor_index];
      cursor_target_x = FP(drivers_window_target_x[cursor_index] + nudge_x, 0);
      cursor_target_y = FP(drivers_window_target_y[cursor_index] + nudge_y, 0);
    } else if (pad1_new & PAD_DOWN) {
      cursor_index = drivers_window_down[cursor_index];
      cursor_target_x = FP(drivers_window_target_x[cursor_index] + nudge_x, 0);
      cursor_target_y = FP(drivers_window_target_y[cursor_index] + nudge_y, 0);
    } else if (pad1_new & PAD_LEFT) {
      cursor_index = drivers_window_left[cursor_index];
      cursor_target_x = FP(drivers_window_target_x[cursor_index] + nudge_x, 0);
      cursor_target_y = FP(drivers_window_target_y[cursor_index] + nudge_y, 0);
    } else if (pad1_new & PAD_RIGHT) {
      cursor_index = drivers_window_right[cursor_index];
      cursor_target_x = FP(drivers_window_target_x[cursor_index] + nudge_x, 0);
      cursor_target_y = FP(drivers_window_target_y[cursor_index] + nudge_y, 0);
    }

    if (cursor_target_x == cursor_x && cursor_target_y == cursor_y) {
      if (pad1_new & PAD_A) {
        current_cursor_state = Clicking;
        cursor_counter = CLICK_DELAY;
      }
    } else {
      set_cursor_speed();
    }
  }
}

void drivers_window_loading_handler () {
  // started loading
  switch(cursor_index) {
  case 0: // Close button
    if (cursor_counter == 0) {
      set_unrle_buffer(unrle_buffer);
      unrle_to_buffer(main_window_nametable);

      set_nametable_loader_buffer(unrle_buffer);
      if (current_screen == 0) {
        start_nametable_loader(NTADR_C(0, 0));
      } else {
        start_nametable_loader(NTADR_A(0, 0));
      }
    } else {
      if (!yield_nametable_loader()) {
        current_cursor_state = Default;
        current_game_state = MainWindow;
        flip_screen();
      }
    }
    break;
  default: // Load driver dungeon
    current_game_state = Dungeon;
    current_cursor_state = Default;
    oam_clear();
    pal_fade_to(4, 0);
    ppu_off();

    set_chr_mode_2(BG_DUNGEON_0);
    set_chr_mode_3(BG_DUNGEON_1);
    set_chr_mode_4(BG_DUNGEON_2);
    set_chr_mode_5(BG_DUNGEON_3);
    set_chr_mode_0(SPRITE_PLAYERS_0);
    set_chr_mode_1(SPRITE_PLAYERS_1);

    pal_bg(dungeon_bg_palette);
    pal_spr(sprites_palette);

    // draw some things
    vram_adr(NTADR_A(0,0));
    vram_unrle(dungeon_hud_nametable);
    set_scroll_x(0);
    set_scroll_y(0);
    current_screen = 0;

    ppu_on_all();
    pal_fade_to(0, 4);

    start_dungeon(cursor_index - 1);
    break;
  }
}

// ::CURSOR::

void draw_cursor() {
  switch (current_cursor_state) {
  case Default:
    oam_meta_spr(INT(cursor_x), INT(cursor_y), default_cursor_sprite);
    break;
  case Clicking:
    if (cursor_counter < (CLICK_DELAY >> 2)) {
      oam_meta_spr(INT(cursor_x), INT(cursor_y), default_cursor_sprite);
    } else {
      oam_meta_spr(INT(cursor_x), INT(cursor_y), clicking_cursor_sprite);
    }
    break;
  case Loading:
    oam_meta_spr(INT(cursor_x), INT(cursor_y), default_cursor_sprite);
    oam_meta_spr(INT(cursor_x) + 0x14, INT(cursor_y) + 0x10, loading_cursor_sprite);
    break;
  }
}

void reset_cursor () {
  cursor_index = 0;
  cursor_x = cursor_target_x = FP(0x80, 0x00);
  cursor_y = cursor_target_y = FP(0x20, 0x00);
  cursor_dx = cursor_dy = 0;
  current_cursor_state = Default;
}

void set_cursor_speed () {
  cursor_dx = cursor_target_x - cursor_x;
  cursor_dy = cursor_target_y - cursor_y;
  while (cursor_dx > MAX_CURSOR_DX || cursor_dy > MAX_CURSOR_DY || cursor_dx < -MAX_CURSOR_DX || cursor_dy < -MAX_CURSOR_DY) {
    if (cursor_dx > MIN_CURSOR_DX || cursor_dx < -MIN_CURSOR_DX) cursor_dx >>= 1;
    if (cursor_dy > MIN_CURSOR_DY || cursor_dy < -MIN_CURSOR_DY) cursor_dy >>= 1;
  }
}

void update_cursor () {
  switch (current_cursor_state) {
  case Default:
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
    break;
  case Clicking:
    --cursor_counter;
    if (cursor_counter == 0) {
      current_cursor_state = Loading;
    }
    break;
  case Loading:
    ++cursor_counter;
    break;
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
  vram_unrle(main_window_nametable);

  if (!dungeon_layout_initialized) {
    dungeon_layout_initialized = 1;
    generate_layout(wram_dungeon_layout);
  }

  // TODO initialize later, maybe on config
  if (!party_initialized) {
    party_initialized = 1;
    initialize_party();
  }

  ppu_on_all();

  pal_fade_to(0, 4);
  current_game_state = MainWindow;

  reset_cursor();

  current_screen = 0;
}

void init_wram (void) {
  if (wram_start != WRAM_VERSION)
    memfill(&wram_start,0,0x2000);
  wram_start = WRAM_VERSION;
}

void flip_screen (void) {
  if (current_screen == 0) {
    current_screen = 1;
    set_scroll_y(0x100);
  } else {
    current_screen = 0;
    set_scroll_y(0x000);
  }
}
