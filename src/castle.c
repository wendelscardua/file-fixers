#include "lib/neslib.h"
#include "castle.h"
#include "irq_buffer.h"
#include "../assets/sprites.h"
#define FP(integer,fraction) (((integer)<<8)|((fraction)>>0))
#define INT(unsigned_fixed_point) ((unsigned_fixed_point>>8)&0xff)

#define START_Y FP(0x78, 0x00)
#define END_Y FP(0x54, 0x00)
#define Y_STEP FP(0x00, 0x80)
#define DIALOG_SCANLINE 0xa0

typedef enum {
              Coming,
              Listening,
              Turning,
              Going
} cutscene_state_t;

#pragma bss-name(push, "ZEROPAGE")
unsigned int player_y;
cutscene_state_t cutscene_state;
#pragma bss-name(pop)

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

void init_castle_cutscene() {
  player_y = START_Y;
  cutscene_state = Coming;
}

void return_from_castle(void);

void castle_handler() {
  switch (cutscene_state) {
  case Coming:
    if (player_y == END_Y) {
      cutscene_state = Listening;
    } else {
      player_y -= Y_STEP;
    }
    break;
  case Listening:
    double_buffer[double_buffer_index++] = DIALOG_SCANLINE - 1;
    double_buffer[double_buffer_index++] = 0xf6;
    double_buffer[double_buffer_index++] = 8;
    double_buffer[double_buffer_index++] = 0xa0;
    double_buffer[double_buffer_index++] = 0;
    double_buffer[double_buffer_index++] = ((0xa0 & 0xF8) << 2);

    // TODO dialog
    // cutscene_state = Turning;
    break;
  case Turning:
    // TODO turn around slowly
    cutscene_state = Going;
  case Going:
    if (player_y == START_Y) {
      return_from_castle();
    } else {
      player_y += Y_STEP;
    }
  }
}

void draw_castle_sprites() {
  oam_meta_spr(0x70, 0x3e, amda_sprite);
  oam_meta_spr(0x80, 0x3e, intelle_sprite);
  oam_meta_spr(0x68, 0x88, player_sprite[(PLAYER_UP_SPR << 2) | 1]);
  oam_meta_spr(0x78, 0x88, player_sprite[(PLAYER_UP_SPR << 2) | 2]);
  oam_meta_spr(0x88, 0x88, player_sprite[(PLAYER_UP_SPR << 2) | 3]);
  switch(cutscene_state) {
  case Coming:
    if ( player_y == END_Y || player_y == START_Y) {
      oam_meta_spr(0x78, INT(player_y), player_sprite[(PLAYER_UP_SPR << 2)]);
    } else if ( INT(player_y) & 0b1000 ) {
      oam_meta_spr(0x78, INT(player_y), player_sprite[(PLAYER_STEP_1_UP_SPR << 2)]);
    } else {
      oam_meta_spr(0x78, INT(player_y), player_sprite[(PLAYER_STEP_2_UP_SPR << 2)]);
    }
    break;
  case Listening:
    oam_meta_spr(0x78, INT(END_Y), player_sprite[(PLAYER_UP_SPR << 2)]);
    break;
  case Turning:
    oam_meta_spr(0x78, INT(END_Y), player_sprite[(PLAYER_RIGHT_SPR << 2)]);
    break;
  case Going:
    if ( player_y == END_Y || player_y == START_Y) {
      oam_meta_spr(0x78, INT(player_y), player_sprite[(PLAYER_DOWN_SPR << 2)]);
    } else if ( INT(player_y) & 0b1000 ) {
      oam_meta_spr(0x78, INT(player_y), player_sprite[(PLAYER_STEP_1_DOWN_SPR << 2)]);
    } else {
      oam_meta_spr(0x78, INT(player_y), player_sprite[(PLAYER_STEP_2_DOWN_SPR << 2)]);
    }
    break;
  }
}
