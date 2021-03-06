#ifndef _ENTITIES_H_
#define _ENTITIES_H_
#define MAX_ENTITIES 16
#define NORMAL_SPEED 12

#define STATUS_HASTE    0x01
#define STATUS_SLOW     0x02
#define STATUS_PROTECT  0x04
#define STATUS_CONFUSE  0x08
#define STATUS_FREEZE  0x10

#define STATUS_LENGTH (2 + roll_die(4))

#define IS_PLAYER(entity_index) ((entity_index < 4))
#define IS_ENEMY(entity_index) ((entity_index >= 4) && entity_type[entity_index] < Player)
#define IS_ITEM(entity_index) ((entity_index >= 4) && entity_type[entity_index] > Player)

typedef enum {
              Eicar,
              Crypt,
              Buggy,
              Stega,
              Nefet,
              Player,
              Potion,
              Ether,
              Elixir,
              NumEntityTypes
} entity_type_enum;

typedef enum {
              EntityInput,
              EntityMovement,
              EntityMenu,
              EntityPlayAction,
              EntityAskTarget
} entity_state_enum;

void init_entities (unsigned char stairs_x, unsigned char stairs_y);
void entity_handler (void);
void draw_entities (void);
void next_entity (void);
void entity_input_handler (void);
void entity_movement_handler (void);
void entity_action_handler (void);
void entity_menu_handler (void);
void entity_ask_target_handler (void);
unsigned char collides_with_map (void);
unsigned char find_entity();

extern unsigned char current_entity;
#pragma zpsym("current_entity");

extern unsigned char entity_turn_counter[];
#endif
