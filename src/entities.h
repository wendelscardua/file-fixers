#ifndef _ENTITIES_H_
#define _ENTITIES_H_
#define MAX_ENTITIES 16
#define NORMAL_SPEED 12

typedef enum {
              Eicar,
              Crypt,
              Buggy,
              Stega,
              Nefet,
              Player
} entity_type_enum;

typedef enum {
              EntityInput,
              EntityMovement,
              EntityAction
} entity_state_enum;

void init_entities (unsigned char stairs_x, unsigned char stairs_y);
void entity_handler (void);
void draw_entities (void);
void next_entity (void);
void entity_input_handler (void);
void entity_movement_handler (void);
void entity_action_handler (void);

extern unsigned char party_level;
#endif
