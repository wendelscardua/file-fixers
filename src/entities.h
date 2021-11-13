#define MAX_ENTITIES 8

typedef enum {
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
