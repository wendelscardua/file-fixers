#include "directions.h"
#include "entities.h"
#include "players.h"
#include "lib/neslib.h"
#include "lib/nesdoug.h"
#include "../assets/sprites.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
extern unsigned char i, temp, temp_x, temp_y, pad1_new;
extern unsigned int temp_int;

unsigned char num_entities;
unsigned char speed_cap;
unsigned char entity_aux;

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
entity_type_enum entity_type[MAX_ENTITIES];
unsigned char entity_turn_counter[MAX_ENTITIES];
unsigned char entity_speed[MAX_ENTITIES];
unsigned char current_entity;
unsigned char current_entity_steps;
entity_state_enum current_entity_state;
unsigned char entity_x, entity_y;

void init_entities(unsigned char stairs_row, unsigned char stairs_col) {
  num_entities = 4;
  current_entity = 0;
  for(i = 0; i < 4; i++) {
    entity_speed[i] = player_dex[i];
    entity_type[i] = Player;
    entity_direction[i] = Down;
  }

  entity_col[0] = entity_col[2] = stairs_col;
  entity_row[1] = entity_row[3] = stairs_row;
  entity_row[0] = stairs_row - 1;
  entity_row[2] = stairs_row + 1;
  entity_col[1] = stairs_col - 1;
  entity_col[3] = stairs_col + 1;

  // TODO get enemies based on sector

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

      entity_row[current_entity] = temp_y;
      entity_col[current_entity] = temp_x;

      current_entity_state = EntityMovement;
      entity_aux = 0x10;
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

  speed_cap = 0;
  for(i = 0; i < num_entities; i++) {
    if (entity_speed[i] > speed_cap) speed_cap = entity_speed[i];
  }

  while(1) {
    entity_turn_counter[current_entity] += entity_speed[current_entity];
    if (entity_turn_counter[current_entity] >= speed_cap) {
      entity_turn_counter[current_entity] -= speed_cap;
      current_entity_steps = 3; // TODO: base on dex maybe?
      current_entity_state = EntityInput;

      entity_x = entity_col[current_entity] * 0x10 + 0x20;
      entity_y = entity_row[current_entity] * 0x10 + 0x20 - 1;
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
        if (entity_aux & 0x10) {
          temp += (1 << 2);
        }
        oam_meta_spr(entity_x, entity_y, player_sprite[temp]);
        break;
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
      }
    }
  }
}
