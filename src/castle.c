#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "mmc3/mmc3_code.h"
#include "charmap.h"
#include "castle.h"
#include "irq_buffer.h"
#include "main.h"
#include "temp.h"
#include "wram.h"
#include "../assets/dialogs.h"
#include "../assets/sprites.h"
#define FP(integer,fraction) (((integer)<<8)|((fraction)>>0))
#define INT(unsigned_fixed_point) ((unsigned_fixed_point>>8)&0xff)

#define START_Y FP(0x78, 0x00)
#define END_Y FP(0x54, 0x00)
#define Y_STEP FP(0x00, 0x80)
#define DIALOG_SCANLINE 0xa0

#define FIRST_DIALOG_ROW 22
#define FIRST_DIALOG_COLUMN 6
#define LAST_DIALOG_ROW 25
#define LAST_DIALOG_COLUMN 28

typedef enum {
              Coming,
              Listening,
              Turning,
              Going
} cutscene_state_t;

#pragma bss-name(push, "ZEROPAGE")
unsigned int player_y;
cutscene_state_t cutscene_state;
char * current_dialog;
unsigned char current_speaker;
unsigned char dialog_row;
unsigned char dialog_column;

#pragma bss-name(pop)
char * dialog_queue[5];
unsigned char dialog_queue_index;

#pragma rodata-name ("BANK1")
#pragma code-name ("BANK1")

void init_castle_cutscene() {
  player_y = START_Y;
  cutscene_state = Coming;
  dialog_queue_index = 0;
  if (dialogs_checklist == 0) {
    current_dialog = dialog_beginning;
    dialog_queue[0] = 0;
    dialogs_checklist = 0x80;
  } else if (yendors == 0xff) {
    current_dialog = dialog_the_end;
    dialog_queue[0] = 0;
    dialogs_checklist = 0xff;
  } else {
    temp = (yendors & ~dialogs_checklist) & 0x0f;
    if (temp == 0) {
      current_dialog = dialog_no_dlls;
      dialog_queue[0] = 0;
    } else {
      i = 0;
      if (temp & 0b0001) {
        dialog_queue[i++] = dialog_gpu_dll;
      }
      if (temp & 0b0010) {
        dialog_queue[i++] = dialog_disk_dll;
      }
      if (temp & 0b0100) {
        dialog_queue[i++] = dialog_io_dll;
      }
      if (temp & 0b1000) {
        dialog_queue[i++] = dialog_ram_dll;
      }
      dialog_queue[i++] = 0;
      current_dialog = dialog_queue[0];
      dialog_queue_index = 1;
      dialogs_checklist |= temp;
    }
  }
  current_speaker = *current_dialog;
  dialog_row = FIRST_DIALOG_ROW;
  dialog_column = FIRST_DIALOG_COLUMN;
}

const char blank[] = "                      ";

void clean_dialog_window() {
  clear_vram_buffer();
  multi_vram_buffer_horz(blank, 22, NTADR_C(6, 22));
  multi_vram_buffer_horz(blank, 22, NTADR_C(6, 23));
  ppu_wait_nmi();
  clear_vram_buffer();
  multi_vram_buffer_horz(blank, 22, NTADR_C(6, 24));
  multi_vram_buffer_horz(blank, 22, NTADR_C(6, 25));
  ppu_wait_nmi();
  clear_vram_buffer();
  dialog_row = FIRST_DIALOG_ROW;
  dialog_column = FIRST_DIALOG_COLUMN;
}

void castle_handler() {
  set_scroll_x(0);
  set_scroll_y(0);
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

    temp = *current_dialog;
    if (temp == 0) {
      pad_poll(0);
      pad1_new = get_pad_new(0);
      if (pad1_new & PAD_A) {
        if (dialog_queue[dialog_queue_index]) {
          current_dialog = dialog_queue[dialog_queue_index];
          dialog_queue_index++;
          clean_dialog_window();
          current_speaker = *current_dialog;
          ++current_dialog;
        } else {
          cutscene_state = Turning;
        }
      }
    } else if (temp <= 2) {
      if (temp != current_speaker) {
        pad_poll(0);
        pad1_new = get_pad_new(0);
        if (pad1_new & PAD_A) {
          current_speaker = temp;
          clean_dialog_window();
          ++current_dialog;
        }
      } else {
        ++current_dialog;
      }
    } else if (temp <= 4) {
      ++dialog_column;
      ++current_dialog;
    } else {
      temp = 0;
      while(current_dialog[temp] > 4) temp++;
      if (dialog_column + temp > LAST_DIALOG_COLUMN) {
        if (dialog_row == LAST_DIALOG_ROW) {
          pad_poll(0);
          pad1_new = get_pad_new(0);
          if (pad1_new & PAD_A) {
            clean_dialog_window();
          }
        } else {
          ++dialog_row;
          dialog_column = FIRST_DIALOG_COLUMN;
        }
      } else {
        clear_vram_buffer();
        multi_vram_buffer_horz(current_dialog, temp, NTADR_C(dialog_column, dialog_row));
        ppu_wait_nmi();
        dialog_column += temp;
        current_dialog += temp;
      }
    }
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

#pragma rodata-name ("RODATA")
#pragma code-name ("CODE")

void draw_castle_sprites() {
  temp_bank = change_prg_8000(0);
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
    if (current_speaker == 1) {
      oam_meta_spr(0x18, 0xb0, amda_sprite);
    } else if (current_speaker == 2) {
      oam_meta_spr(0x18, 0xb0, intelle_sprite);
    }
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
  set_prg_8000(temp_bank);
}
