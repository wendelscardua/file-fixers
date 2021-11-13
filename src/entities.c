#include "directions.h"
#include "entities.h"
#include "players.h"
#include "../assets/sprites.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
extern unsigned char i, temp, temp_x, temp_y;
extern unsigned int temp_int;

unsigned char num_entities;
unsigned char speed_cap;
unsigned char entity_aux;

#pragma zpsym("i");
#pragma zpsym("temp");
#pragma zpsym("temp_x");
#pragma zpsym("temp_y");
#pragma zpsym("temp_int");

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
}

void entity_handler() {
  if (num_entities == 0) return;
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
      break;
    }
    ++current_entity;
    if (current_entity >= num_entities) {
      current_entity = 0;
    }
  }
}
