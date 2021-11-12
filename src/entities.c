#include "entities.h"
#include "../assets/sprites.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")
extern unsigned char temp, temp_x, temp_y;
extern unsigned int temp_int;

unsigned char num_entities;

#pragma zpsym("temp");
#pragma zpsym("temp_x");
#pragma zpsym("temp_y");
#pragma zpsym("temp_int");

#pragma bss-name(pop)

unsigned char entity_x[MAX_ENTITIES];
unsigned char entity_y[MAX_ENTITIES];
entity_type_enum entity_type[MAX_ENTITIES];
unsigned char entity_turn_counter[MAX_ENTITIES];
unsigned char entity_speed[MAX_ENTITIES];
unsigned char current_entity;
entity_state_enum current_entity_state;

void init_entities() {
  num_entities = 0;
  current_entity = 0;
}

void entity_handler() {
  if (num_entities == 0) return;
}

void next_entity() {
  if (num_entities == 0) return;

  while(1) {
    if (entity_turn()) {
      break;
    }
    ++current_entity;
    if (current_entity >= num_entities) {
      current_entity = 0;
    }
  }
}