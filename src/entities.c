#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "lib/subrand.h"
#include "directions.h"
#include "dungeon.h"
#include "enemies.h"
#include "entities.h"
#include "wram.h"
#include "../assets/enemy-stats.h"
#include "../assets/sprites.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
extern unsigned char i, temp, temp_x, temp_y, pad1_new;
extern unsigned int temp_int;

unsigned char num_entities;
unsigned char entity_aux;
unsigned char temp_w, temp_h;
unsigned char *room_ptr;

#pragma zpsym("i");
#pragma zpsym("temp");
#pragma zpsym("temp_x");
#pragma zpsym("temp_y");
#pragma zpsym("temp_int");
#pragma zpsym("pad1_new");

#pragma bss-name(pop)

unsigned char entity_row[MAX_ENTITIES];
unsigned char entity_col[MAX_ENTITIES];
direction entity_direction[MAX_ENTITIES];
unsigned char entity_turn_counter[MAX_ENTITIES];
unsigned char current_entity;
unsigned char current_entity_moves;
entity_state_enum current_entity_state;
unsigned char entity_x, entity_y;

void refresh_moves_hud() {
  temp = current_entity_moves;
  temp_x = 0;
  while (temp >= 10) {
    temp -= 10;
    ++temp_x;
  }
  if (temp_x > 0) {
    one_vram_buffer(0x10 + temp_x, NTADR_A(22, 27));
  } else {
    one_vram_buffer(0x00, NTADR_A(22, 27));
  }
  one_vram_buffer(0x10 + temp, NTADR_A(23, 27));
}

void refresh_hud() {
  refresh_moves_hud();

  if (entity_type[current_entity] == Player) {
    multi_vram_buffer_horz((char *) player_name[current_entity], 5, NTADR_A(3, 25));
  } else {
    multi_vram_buffer_horz((char *) enemy_name[entity_type[current_entity]], 5, NTADR_A(3, 25));
  }
}

void init_entities(unsigned char stairs_row, unsigned char stairs_col) {
  for(num_entities = 0; num_entities < 4; num_entities++) {
    entity_direction[num_entities] = Down;
  }
  entity_col[0] = entity_col[2] = stairs_col;
  entity_row[1] = entity_row[3] = stairs_row;
  entity_row[0] = stairs_row - 1;
  entity_row[2] = stairs_row + 1;
  entity_col[1] = stairs_col - 1;
  entity_col[3] = stairs_col + 1;

  while (num_entities == 4) {
    room_ptr = current_sector_room_data;
    while((temp_x = *room_ptr) != 0xff) {
      ++room_ptr;
      temp_y = *room_ptr;
      ++room_ptr;
      temp_w = *room_ptr;
      ++room_ptr;
      temp_h = *room_ptr;
      ++room_ptr;

      if (subrand8(2) == 0) { // 33% chance to spawn in room
        entity_col[num_entities] = temp_x + subrand8(temp_w);
        entity_row[num_entities] = temp_y + subrand8(temp_h);
        entity_type[num_entities] = temp = select_enemy_type();
        entity_moves[num_entities] = enemy_moves[temp];
        entity_speed[num_entities] = enemy_speed[temp];
        num_entities++;
      }
    }
  }

  current_entity = num_entities - 1;
  next_entity();
}

void entity_handler() {
  if (num_entities == 0) return;
  switch(current_entity_state) {
  case EntityInput:
    entity_input_handler();
    break;
  case EntityMovement:
    entity_movement_handler();
    break;
  case EntityAction:
    entity_action_handler();
    break;
  }
}

extern unsigned char * current_sector;
unsigned char entity_collides() {
  if (temp_x == 0xff || temp_y == 0xff || temp_x == 12 || temp_y == 10) return 1;

  if (current_sector[temp_y * 12 + temp_x] == NullMetatile) return 1;

  for(i = 0; i < num_entities; i++) {
    if (entity_col[i] == temp_x && entity_row[i] == temp_y) return 1;
  }

  return 0;
}

void entity_input_handler() {
  switch(entity_type[current_entity]) {
  case Player:
    pad_poll(0);
    pad1_new = get_pad_new(0);
    if (pad1_new & (PAD_UP|PAD_DOWN|PAD_LEFT|PAD_RIGHT)) {
      temp_x = entity_col[current_entity];
      temp_y = entity_row[current_entity];
      if (pad1_new & PAD_UP) { --temp_y; temp = entity_direction[current_entity] = Up; }
      if (pad1_new & PAD_DOWN) { ++temp_y; temp = entity_direction[current_entity] = Down; }
      if (pad1_new & PAD_LEFT) { --temp_x; temp = entity_direction[current_entity] = Left; }
      if (pad1_new & PAD_RIGHT) { ++temp_x; temp = entity_direction[current_entity] = Right; }

      if (current_entity_moves > 0 && !entity_collides()) {
        entity_row[current_entity] = temp_y;
        entity_col[current_entity] = temp_x;
        --current_entity_moves;
        refresh_moves_hud();
        current_entity_state = EntityMovement;
        entity_aux = 0x10;
      }
    } else if (pad1_new & PAD_B) { // Pass
      next_entity();
    }
    break;
  default:
    // Random walk
    if (current_entity_moves > 0 && entity_aux < 0x10) {
      temp_x = entity_col[current_entity];
      temp_y = entity_row[current_entity];
      temp = entity_direction[current_entity] = subrand8(3);
      switch(temp) {
      case Up: --temp_y; break;
      case Down: ++temp_y; break;
      case Left: --temp_x; break;
      case Right: ++temp_x; break;
      }

      if (!entity_collides()) {
        entity_row[current_entity] = temp_y;
        entity_col[current_entity] = temp_x;
        --current_entity_moves;
        refresh_moves_hud();
        current_entity_state = EntityMovement;
        entity_aux = 0x10;
      } else {
        ++entity_aux;
      }
    } else {
      // TODO add actions
      next_entity();
    }
    break;
  }
}

void entity_movement_handler() {
  --entity_aux;
  switch (entity_direction[current_entity]) {
  case Up: --entity_y; break;
  case Down: ++entity_y; break;
  case Left: --entity_x; break;
  case Right: ++entity_x; break;
  }
  if (entity_aux == 0) {
    current_entity_state = EntityInput;
  }
}

void entity_action_handler() {
}

void next_entity() {
  if (num_entities == 0) return;

  ++current_entity;
  if (current_entity >= num_entities) {
    current_entity = 0;
  }

  while(1) {
    entity_turn_counter[current_entity] += entity_speed[current_entity];
    if (entity_turn_counter[current_entity] >= NORMAL_SPEED) {
      entity_turn_counter[current_entity] -= NORMAL_SPEED;
      current_entity_moves = entity_moves[current_entity];
      current_entity_state = EntityInput;
      entity_aux = 0;

      entity_x = entity_col[current_entity] * 0x10 + 0x20;
      entity_y = entity_row[current_entity] * 0x10 + 0x20 - 1;

      refresh_hud();
      break;
    }
    ++current_entity;
    if (current_entity >= num_entities) {
      current_entity = 0;
    }
  }
}

void draw_entities() {
  for(i = 0; i < num_entities; ++i) {
    if (i == current_entity && current_entity_state == EntityMovement) {
      switch(entity_type[i]) {
      case Player:
        switch(entity_direction[i]) {
        case Up: temp = (PLAYER_STEP_1_UP_SPR << 2) | i; break;
        case Down: temp = (PLAYER_STEP_1_DOWN_SPR << 2) | i; break;
        case Left: temp = (PLAYER_STEP_1_LEFT_SPR << 2) | i; break;
        case Right: temp = (PLAYER_STEP_1_RIGHT_SPR << 2) | i; break;
        }
        if (entity_aux & 0b1000) {
          temp += (1 << 2);
        }
        oam_meta_spr(entity_x, entity_y, player_sprite[temp]);
        break;
      default: // enemies
        switch(entity_direction[i]) {
        case Up: temp = ENEMY_LEFT_1_SPR; break;
        case Down: temp = ENEMY_RIGHT_1_SPR; break;
        case Left: temp = ENEMY_LEFT_1_SPR; break;
        case Right: temp = ENEMY_RIGHT_1_SPR; break;
        }
        if (entity_aux & 0b1000) {
          temp++;
        }
        oam_meta_spr(entity_x,
                     entity_y,
                     enemy_sprite[
                                  enemy_sprite_index[entity_type[i]] | temp
                                  ]);
      }
    } else {
      temp_x = entity_col[i] * 0x10 + 0x20;
      temp_y = entity_row[i] * 0x10 + 0x20 - 1;
      switch(entity_type[i]) {
      case Player:
        switch(entity_direction[i]) {
        case Up: temp = (PLAYER_UP_SPR << 2) | i; break;
        case Down: temp = (PLAYER_DOWN_SPR << 2) | i; break;
        case Left: temp = (PLAYER_LEFT_SPR << 2) | i; break;
        case Right: temp = (PLAYER_RIGHT_SPR << 2) | i; break;
        }
        oam_meta_spr(temp_x, temp_y, player_sprite[temp]);

        break;
      default: // enemies
        switch(entity_direction[i]) {
        case Up: temp = ENEMY_LEFT_2_SPR; break;
        case Down: temp = ENEMY_RIGHT_2_SPR; break;
        case Left: temp = ENEMY_LEFT_1_SPR; break;
        case Right: temp = ENEMY_RIGHT_1_SPR; break;
        }
        oam_meta_spr(temp_x,
                     temp_y,
                     enemy_sprite[
                                  enemy_sprite_index[entity_type[i]] | temp
                                  ]);
      }
    }
  }
}
