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
unsigned char __fastcall__ entity_turn (void);
