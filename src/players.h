#ifndef _PLAYERS_H_
#define _PLAYERS_H_
typedef enum {
              Fighter,
              Mage,
              Support
} player_class_type;

void initialize_party(void);
#endif
