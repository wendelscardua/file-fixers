/* Based on ...
 *  example of MMC3 for cc65
 *	Doug Fraker 2019
 */
#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "lib/unrle.h"
#include "mmc3/mmc3_code.h"
#include "music/soundtrack.h"
#include "charmap.h"
#include "castle.h"
#include "dungeon.h"
#include "irq_buffer.h"
#include "nametable_loader.h"
#include "players.h"
#include "temp.h"
#include "wram.h"
#include "../assets/nametables.h"
#include "../assets/palettes.h"
#include "../assets/sectors.h"
#include "../assets/sprites.h"

#define FP(integer,fraction) (((integer)<<8)|((fraction)>>0))
#define INT(unsigned_fixed_point) ((unsigned_fixed_point>>8)&0xff)

#define NTADR_AUTO(x, y) (current_screen == 0 ? (NTADR_A((x), (y))) : (NTADR_C((x), (y))))
#define NTADR_ALT_AUTO(x, y) (current_screen == 0 ? (NTADR_C((x), (y))) : (NTADR_A((x), (y))))

#define CONFIG_ORIG_X 4
#define CONFIG_ORIG_Y 8

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
unsigned char unseeded;

// Game stuff
enum game_state {
                 Title,
                 MainWindow,
                 DriversWindow,
                 ConfigWindow,
                 AboutWindow,
                 Dungeon,
                 Castle,
                 GameOver
} current_game_state;

unsigned char cursor_index, cursor_counter;
unsigned int cursor_x, cursor_y, cursor_target_x, cursor_target_y;
signed int cursor_dx, cursor_dy;
unsigned char keyboard_loaded, keyboard_row, keyboard_column, keyboard_active;
unsigned char input_length;
unsigned char * input_field;
unsigned char keyboard_scanline;

enum cursor_state {
                   Default,
                   Clicking,
                   Loading,
                   Disabled
} current_cursor_state;

unsigned char current_screen;

#pragma bss-name(pop)
// should be in the regular 0x300 ram now

// the fixed bank

#pragma rodata-name ("RODATA")
#pragma code-name ("STARTUP")

void about_window_default_cursor_handler (void);
void about_window_loading_handler (void);
void about_window_handler (void);
void config_window_default_cursor_handler (void);
void config_window_keyboard_handler (void);
void config_window_loading_handler (void);
void config_window_handler (void);
void draw_cursor (void);
void draw_about_window_sprites (void);
void draw_config_window_sprites (void);
void draw_drivers_window_sprites (void);
void draw_main_window_sprites (void);
void draw_sprites (void);
void draw_title_sprites (void);
void drivers_window_default_cursor_handler (void);
void drivers_window_loading_handler (void);
void drivers_window_handler (void);
void flip_screen (void);
void game_over_handler (void);
void go_to_castle (void);
void go_to_game_over (void);
void go_to_title (void);
void init_wram (void);
void main_window_default_cursor_handler (void);
void main_window_loading_handler (void);
void main_window_handler (void);
void refresh_config_classes (void);
void refresh_config_names (void);
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

    rand16();

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
    case ConfigWindow:
      config_window_handler();
      break;
    case AboutWindow:
      about_window_handler();
      break;
    case Dungeon:
      dungeon_handler();
      break;
    case Castle:
      castle_handler();
    case GameOver:
      game_over_handler();
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
  case ConfigWindow:
    draw_config_window_sprites();
    break;
  case AboutWindow:
    draw_about_window_sprites();
    break;
  case Dungeon:
    draw_dungeon_sprites();
    break;
  case Castle:
    draw_castle_sprites();
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
  music_play(InevitableDoom);

  set_scroll_x(0);
  set_scroll_y(0);

  draw_sprites();

  set_chr_mode_2(BG_MAIN_0);
  set_chr_mode_3(BG_MAIN_1);
  set_chr_mode_4(BG_MAIN_2);
  set_chr_mode_5(BG_MAIN_3);
  set_chr_mode_0(SPRITE_0);
  set_chr_mode_1(SPRITE_1);

  pal_bg(bg_palette);
  pal_spr(sprites_palette);

  ppu_on_all(); //	turn on screen
  pal_fade_to(0, 4);
}

void title_handler() {
  for(i = 0; i < 16; i++) {
    pad_poll(0);
    rand16();
    pad1_new = get_pad_new(0);
    if (pad1_new & (PAD_START | PAD_A)) {
      sfx_play(SFX_START, 0);
      start_game();
      break;
    }
#ifdef DEBUG
    if (pad1_new & (PAD_SELECT)) {
      sfx_play(SFX_START, 0);
      go_to_game_over();
      break;
    }
#endif
  }
}

void draw_title_sprites() {
}

// ::MAIN WINDOW::

void main_window_handler() {
  pad_poll(0);
  pad1_new = get_pad_new(0);

  switch(current_cursor_state) {
  case Default:
  case Disabled:
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

// none, castle, drivers, about, config
const unsigned char main_window_target_x[] = { 0x20, 0x48, 0x48, 0xa8, 0x48 };
const unsigned char main_window_target_y[] = { 0x20, 0x58, 0x78, 0x78, 0x98 };
const unsigned char main_window_up[]       = {    1,    1,    1,    1,    2 };
const unsigned char main_window_down[]     = {    1,    2,    4,    4,    4 };
const unsigned char main_window_left[]     = {    1,    1,    2,    2,    4 };
const unsigned char main_window_right[]    = {    1,    3,    3,    3,    3 };

void main_window_default_cursor_handler() {
  if (cursor_target_x == cursor_x && cursor_target_y == cursor_y
      && ((cursor_index == 2 && dialogs_checklist == 0)
          || (cursor_index == 1 && party_initialized == 0))) {
    current_cursor_state = Disabled;
  }

  if (pad1_new) {
    signed char nudge_x = rand8() % 4 - 2;
    signed char nudge_y = rand8() % 4 - 2;
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
      if ((pad1_new & PAD_A) && current_cursor_state == Default) {
        current_cursor_state = Clicking;
        cursor_counter = CLICK_DELAY;
      }
    } else {
      current_cursor_state = Default;
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
    go_to_castle();
    break;
  case 2: // Drivers
    if (cursor_counter == 0) {
      set_unrle_buffer(unrle_buffer);
      unrle_to_buffer(drivers_window_nametable);

      set_nametable_loader_buffer(unrle_buffer);
      start_nametable_loader(NTADR_ALT_AUTO(0, 0));
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
    if (cursor_counter == 0) {
      set_unrle_buffer(unrle_buffer);
      unrle_to_buffer(about_window_nametable);

      set_nametable_loader_buffer(unrle_buffer);
      start_nametable_loader(NTADR_ALT_AUTO(0, 0));
    } else {
      if (!yield_nametable_loader()) {
        current_cursor_state = Default;
        cursor_index = 0;
        cursor_target_x = FP(0xbc, 0);
        cursor_target_y = FP(0x24, 0);
        set_cursor_speed();
        current_game_state = AboutWindow;
        flip_screen();
      }
    }
    break;
  case 4: // Config.sys
    if (cursor_counter == 0) {
      set_unrle_buffer(unrle_buffer);
      unrle_to_buffer(config_window_nametable);

      set_nametable_loader_buffer(unrle_buffer);
      start_nametable_loader(NTADR_ALT_AUTO(0, 0));
    } else {
      if (!yield_nametable_loader()) {
        current_cursor_state = Default;
        cursor_index = 0;
        cursor_target_x = FP(0xbc, 0);
        cursor_target_y = FP(0x44, 0);
        set_cursor_speed();
        current_game_state = ConfigWindow;
        for(i = 0; i < 4; i++) {
          temp = entity_lv[i];
          if (temp == 0) temp = 1;
          one_vram_buffer(0x10 + (temp / 10), NTADR_ALT_AUTO(CONFIG_ORIG_X + 16, CONFIG_ORIG_Y + 2 + 2 * i));
          one_vram_buffer(0x10 + (temp % 10), NTADR_ALT_AUTO(CONFIG_ORIG_X + 17, CONFIG_ORIG_Y + 2 + 2 * i));
        }
        set_unrle_buffer(unrle_buffer);
        unrle_to_buffer(keyboard_nametable);
        set_nametable_loader_buffer(unrle_buffer);
        flip_screen();
        start_nametable_loader(NTADR_ALT_AUTO(0, 0));
        refresh_config_classes();
        refresh_config_names();
        keyboard_loaded = 0;
        keyboard_row = 0;
        keyboard_column = 0;
        keyboard_active = 0;
        set_chr_mode_1(SPRITE_PLAYERS_0);
      }
    }
    break;
  }
}

// ::DRIVERS WINDOW::

void drivers_window_handler() {
  pad_poll(0);
  pad1_new = get_pad_new(0);

  switch(current_cursor_state) {
  case Default:
  case Disabled:
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

// close, top left, top right, bottom left, bottom right
const unsigned char drivers_window_target_x[] = { 0xbc, 0x38, 0x78, 0x38, 0x78 };
const unsigned char drivers_window_target_y[] = { 0x44, 0x58, 0x58, 0x88, 0x88 };
const unsigned char drivers_window_up[]       = {    0,    0,    0,    1,    2 };
const unsigned char drivers_window_down[]     = {    2,    3,    4,    3,    4 };
const unsigned char drivers_window_left[]     = {    2,    1,    1,    3,    3 };
const unsigned char drivers_window_right[]    = {    0,    2,    0,    4,    0 };

void drivers_window_default_cursor_handler() {
  if (cursor_target_x == cursor_x && cursor_target_y == cursor_y
      && cursor_index > 0 && dungeon_completed(cursor_index - 1)) {
    current_cursor_state = Disabled;
  }
  if (pad1_new) {
    signed char nudge_x = cursor_index == 0 ? 0 : rand8() % 4 - 2;
    signed char nudge_y = cursor_index == 0 ? 0 : rand8() % 4 - 2;
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
      if ((pad1_new & PAD_A) &&
          (current_cursor_state == Default)) {
        current_cursor_state = Clicking;
        cursor_counter = CLICK_DELAY;
      }
    } else {
      current_cursor_state = Default;
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
    pal_spr(dungeon_sprites_palette);

    // draw some things
    vram_adr(NTADR_A(0,0));
    vram_unrle(dungeon_hud_nametable);
    vram_adr(NTADR_C(0,0));
    vram_unrle(actions_menu_nametable);
    set_scroll_x(0);
    set_scroll_y(0);
    current_screen = 0;

    ppu_on_all();
    pal_fade_to(0, 4);

    // reset players
    for(i = 0; i < 4; i++) {
      entity_hp[i] = entity_max_hp[i];
      player_sp[i] = player_max_sp[i];
      entity_status[i] = 0;
      entity_status_turns[i] = 0;
    }

    start_dungeon(cursor_index - 1);

    music_play(Upbeat);

    break;
  }
}

void return_from_dungeon() {
  current_game_state = DriversWindow;
  if (dungeon_completed(cursor_index - 1)) {
    current_cursor_state = Disabled;
  } else {
    current_cursor_state = Default;
  }
  oam_clear();
  pal_fade_to(4, 0);
  ppu_off();
  set_chr_mode_2(BG_MAIN_0);
  set_chr_mode_3(BG_MAIN_1);
  set_chr_mode_4(BG_MAIN_2);
  set_chr_mode_5(BG_MAIN_3);
  set_chr_mode_0(SPRITE_0);
  set_chr_mode_1(SPRITE_1);
  pal_bg(bg_palette);
  pal_spr(sprites_palette);
  vram_adr(NTADR_A(0,0));
  vram_unrle(drivers_window_nametable);
  set_scroll_x(0);
  set_scroll_y(0);
  current_screen = 0;

  ppu_on_all();
  pal_fade_to(0, 4);
}

// ::CONFIG::

void config_window_handler() {
  pad_poll(0);
  pad1_new = get_pad_new(0);

  if (!keyboard_loaded && !yield_nametable_loader()) {
    keyboard_loaded = 1;
  }

  if (keyboard_active) {
    config_window_keyboard_handler();
    return;
  }

  switch(current_cursor_state) {
  case Default:
  case Disabled:
    config_window_default_cursor_handler();
    break;
  case Loading:
    config_window_loading_handler();
    break;
  }
  update_cursor();
}

// close, edit1, class1a, class1b, ..., class4b
const unsigned char config_window_target_x[] = { 0xbc, 0x6c, 0x44, 0x84, 0x6c, 0x44, 0x84, 0x6c, 0x44, 0x84, 0x6c, 0x44, 0x84 };
const unsigned char config_window_target_y[] = { 0x44, 0x54, 0x5c, 0x5c, 0x64, 0x6c, 0x6c, 0x74, 0x7c, 0x7c, 0x84, 0x8c, 0x8c };
const unsigned char config_window_up[]       = {    0,    0,    1,    1,    3,    4,    4,    6,    7,    7,    9,   10,   10 };
const unsigned char config_window_down[]     = {    1,    3,    4,    4,    6,    7,    7,    9,   10,   10,   12,   12,   12 };
const unsigned char config_window_left[]     = {    1,    2,    2,    2,    5,    5,    5,    8,    8,    8,   11,   11,   11 };
const unsigned char config_window_right[]    = {    0,    3,    3,    3,    6,    6,    6,    9,    9,    9,   12,   12,   12 };

void config_window_default_cursor_handler() {
  if (cursor_target_x == cursor_x && cursor_target_y == cursor_y
      && cursor_index > 0 && party_initialized == 1
      && cursor_index != 1 && cursor_index != 4 && cursor_index != 7 && cursor_index != 10) {
    current_cursor_state = Disabled;
  }
  if (pad1_new) {
    if (pad1_new & PAD_UP) {
      cursor_index = config_window_up[cursor_index];
      cursor_target_x = FP(config_window_target_x[cursor_index], 0);
      cursor_target_y = FP(config_window_target_y[cursor_index], 0);
    } else if (pad1_new & PAD_DOWN) {
      cursor_index = config_window_down[cursor_index];
      cursor_target_x = FP(config_window_target_x[cursor_index], 0);
      cursor_target_y = FP(config_window_target_y[cursor_index], 0);
    } else if (pad1_new & PAD_LEFT) {
      cursor_index = config_window_left[cursor_index];
      cursor_target_x = FP(config_window_target_x[cursor_index], 0);
      cursor_target_y = FP(config_window_target_y[cursor_index], 0);
    } else if (pad1_new & PAD_RIGHT) {
      cursor_index = config_window_right[cursor_index];
      cursor_target_x = FP(config_window_target_x[cursor_index], 0);
      cursor_target_y = FP(config_window_target_y[cursor_index], 0);
    }

    if (cursor_target_x == cursor_x && cursor_target_y == cursor_y) {
      if ((pad1_new & PAD_A) &&
          (current_cursor_state == Default)) {
        current_cursor_state = Clicking;
        cursor_counter = CLICK_DELAY;
      }
    } else {
      current_cursor_state = Default;
      set_cursor_speed();
    }
  }
}

#define KEYBOARD_SCANLINE 0xb0

const unsigned char upper_keys[][] =
  {
   "ABCDEFGHI?",
   "JKLMNOPQR?",
   "STUVWXYZ ?",
   "0123456789"
  };

const unsigned char lower_keys[][] =
  {
   "abcdefghi?",
   "jklmnopqr?",
   "stuvwxyz ?",
   "0123456789"
  };

void config_window_keyboard_handler() {
  if (keyboard_scanline > KEYBOARD_SCANLINE) keyboard_scanline -= 2;
  double_buffer[double_buffer_index++] = keyboard_scanline - 1;
  double_buffer[double_buffer_index++] = 0xf6;
  double_buffer[double_buffer_index++] = (current_screen == 0 ? 8 : 0);
  temp_int = (input_length == 5) ? 0x00 : 0x40;
  double_buffer[double_buffer_index++] = temp_int;
  double_buffer[double_buffer_index++] = 0;
  double_buffer[double_buffer_index++] = ((temp_int & 0xF8) << 2);

  if (keyboard_scanline > KEYBOARD_SCANLINE) return;

  if (pad1_new) {
    if (pad1_new & PAD_UP) {
      if (keyboard_row > 0) --keyboard_row;
      if (keyboard_column == 9) keyboard_column = 8;
    } else if (pad1_new & PAD_DOWN) {
      if (keyboard_row < 3) ++keyboard_row;
    } else if (pad1_new & PAD_LEFT) {
      if (keyboard_column > 0) --keyboard_column;
    } else if (pad1_new & PAD_RIGHT) {
      if (keyboard_column < 8 || (keyboard_row == 3 && keyboard_column == 8)) ++keyboard_column;
    } else if (pad1_new & PAD_A) {
      if (input_length == 5) {
        *input_field = upper_keys[keyboard_row][keyboard_column];
      } else {
        *input_field = lower_keys[keyboard_row][keyboard_column];
      }
      ++input_field;
      --input_length;
      refresh_config_names();
      if (input_length == 0) keyboard_active = 0;
    }
  }
}

void config_window_loading_handler() {
  // started loading
  i = (cursor_index - 1) / 3;

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
        if (!party_initialized) {
          for(i = 0; i < 4; i++) {
            if (player_class[i] == None) return;
          }
          initialize_party();
        }
        set_chr_mode_1(SPRITE_1);
      }
    }
    break;
  case 1:
  case 4:
  case 7:
  case 10:
    input_field = player_name[i];
    for (input_length = 0; input_length < 5; input_length++) {
      player_name[i][input_length] = 0;
    }
    keyboard_active = 1;
    keyboard_scanline = 0xf0;
    current_cursor_state = Default;
    refresh_config_names();
    break;
  case 2:
  case 5:
  case 8:
  case 11:
    if (player_class[i] > None) --player_class[i];
    refresh_config_classes();
    current_cursor_state = Default;
    break;
  case 3:
  case 6:
  case 9:
  case 12:
    if (player_class[i] < Support) ++player_class[i];
    refresh_config_classes();
    current_cursor_state = Default;
    break;
  default:
    break;
  }
}

void draw_config_window_sprites() {
  const unsigned char spin_sequence[] = { 0x80, 0x84, 0x82, 0x84 };

  draw_cursor();

  temp_x = (get_frame_count() & 0b110000) >> 4;
  temp_char = spin_sequence[temp_x];
  temp_attr = 1 | (temp_x <= 1 ? 0 : OAM_FLIP_H);
  temp_x = (temp_x <= 1 ? 0x28 : 0x30);

  temp_y = 0x50 - 1;

  for (i = 0; i < 4; i++) {
    oam_spr(temp_x, temp_y, temp_char, temp_attr);
    oam_spr(0x28 + 0x30 - temp_x, temp_y, temp_char + 0x01, temp_attr);
    oam_spr(temp_x, temp_y + 0x08, temp_char + 0x10, temp_attr);
    oam_spr(0x28 + 0x30 - temp_x, temp_y + 0x08, temp_char + 0x11, temp_attr);
    temp_char += 0x20;
    temp_y += 0x10;
  }

  if (keyboard_active && keyboard_scanline == KEYBOARD_SCANLINE) {
    oam_spr(keyboard_column * 0x10 + 0x30, keyboard_row * 0x08 + 0x08 + KEYBOARD_SCANLINE - 1, 0x24, 1 | OAM_BEHIND);
  }
}

void refresh_config_classes() {
  for(i = 0; i < 4; i++) {
    multi_vram_buffer_horz((char *) class_names[player_class[i]], 7, NTADR_AUTO(CONFIG_ORIG_X + 5, CONFIG_ORIG_Y + 3 + 2 * i));
  }
}

void refresh_config_names() {
  for(i = 0; i < 4; i++) {
    multi_vram_buffer_horz((char *) player_name[i], 5, NTADR_AUTO(CONFIG_ORIG_X + 4, CONFIG_ORIG_Y + 2 + 2 * i));
  }
}

// ::ABOUT::

void about_window_handler() {
  pad_poll(0);
  pad1_new = get_pad_new(0);

  switch(current_cursor_state) {
  case Default:
  case Disabled:
    about_window_default_cursor_handler();
    break;
  case Loading:
    about_window_loading_handler();
    break;
  }

  update_cursor();
}

void draw_about_window_sprites() {
  draw_cursor();
}

void about_window_default_cursor_handler() {
  if (pad1_new) {
    if (cursor_target_x == cursor_x && cursor_target_y == cursor_y) {
      if ((pad1_new & PAD_A) &&
          (current_cursor_state == Default)) {
        current_cursor_state = Clicking;
        cursor_counter = CLICK_DELAY;
      }
    } else {
      current_cursor_state = Default;
      set_cursor_speed();
    }
  }
}

void about_window_loading_handler () {
  // close button
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
  case Disabled:
    oam_meta_spr(INT(cursor_x), INT(cursor_y), disabled_cursor_sprite);
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

// ::GAME OVER::

void game_over_handler() {
  pad_poll(0);
  pad1_new = get_pad_new(0);
  if (pad1_new & PAD_START) {
    dungeon_layout_initialized = 0;
    party_initialized = 0;
    start_game();
  }
}

void go_to_game_over () {
  current_game_state = GameOver;

  if (irq_array[0] != 0xff) {
    while(!is_irq_done() ){}
    irq_array[0] = 0xff;
    double_buffer[0] = 0xff;
  }

  clear_vram_buffer();

  ppu_off(); // screen off
  // draw some things
  vram_adr(NTADR_A(0,0));
  vram_unrle(blue_screen_nametable);
  music_stop();

  set_scroll_x(0);
  set_scroll_y(0);

  oam_clear();

  set_chr_mode_2(BG_MAIN_0);
  set_chr_mode_3(BG_MAIN_1);
  set_chr_mode_4(BG_MAIN_2);
  set_chr_mode_5(BG_MAIN_3);
  set_chr_mode_0(SPRITE_0);
  set_chr_mode_1(SPRITE_1);

  pal_bg(dungeon_bg_palette);
  pal_spr(dungeon_sprites_palette);

  ppu_on_all(); //	turn on screen
  multi_vram_buffer_horz(dungeon_name[current_dungeon_index], 8, NTADR_A(23, 9));
  multi_vram_buffer_horz(dungeon_name[current_dungeon_index], 8, NTADR_A(7, 22));

  music_play(InevitableDoom);
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
    generate_layout();
    yendors = 0;
    dialogs_checklist = 0;
  }

  // TODO initialize later, maybe on config
  if (!party_initialized) {
    if (player_name[0][0] == 0) memcpy(player_name[0], "Lorem", 5);
    if (player_name[1][0] == 0) memcpy(player_name[1], "Ipsum", 5);
    if (player_name[2][0] == 0) memcpy(player_name[2], "Dolor", 5);
    if (player_name[3][0] == 0) memcpy(player_name[3], "Amet ", 5);
    for(i = 0; i < 4; i++) {
      entity_lv[i] = 1;
      player_class[i] = None;
    }
  }

  ppu_on_all();

  pal_fade_to(0, 4);
  current_game_state = MainWindow;

  reset_cursor();

  current_screen = 0;

  music_play(InevitableDoom);
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

void go_to_castle (void) {
  current_game_state = Castle;

  init_castle_cutscene();

  if (irq_array[0] != 0xff) {
    while(!is_irq_done() ){}
    irq_array[0] = 0xff;
    double_buffer[0] = 0xff;
  }

  clear_vram_buffer();

  pal_fade_to(4, 0);
  ppu_off();
  vram_adr(NTADR_A(0,0));
  vram_unrle(castle_nametable);
  vram_adr(NTADR_C(0,0));
  vram_unrle(castle_dialog_nametable);

  set_scroll_x(0);
  set_scroll_y(0);

  set_chr_mode_2(BG_MAIN_0);
  set_chr_mode_3(BG_MAIN_1);
  set_chr_mode_4(BG_MAIN_2);
  set_chr_mode_5(BG_MAIN_3);
  set_chr_mode_0(SPRITE_PLAYERS_0);
  set_chr_mode_1(SPRITE_0);

  pal_bg(castle_bg_palette);
  pal_spr(castle_sprites_palette);

  draw_sprites();

  ppu_on_all();
  pal_fade_to(0, 4);

  if (yendors == 0xff) {
    music_play(Cheery);
  } else {
    music_play(InevitableDoom);
  }
}

void return_from_castle() {
  current_game_state = MainWindow;
  current_cursor_state = Default;
  oam_clear();
  pal_fade_to(4, 0);
  ppu_off();
  set_chr_mode_2(BG_MAIN_0);
  set_chr_mode_3(BG_MAIN_1);
  set_chr_mode_4(BG_MAIN_2);
  set_chr_mode_5(BG_MAIN_3);
  set_chr_mode_0(SPRITE_0);
  set_chr_mode_1(SPRITE_1);
  pal_bg(bg_palette);
  pal_spr(sprites_palette);
  vram_adr(NTADR_A(0,0));
  vram_unrle(main_window_nametable);
  set_scroll_x(0);
  set_scroll_y(0);
  current_screen = 0;

  ppu_on_all();
  pal_fade_to(0, 4);
}
