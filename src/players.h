#ifndef _PLAYERS_H_
#define _PLAYERS_H_
typedef enum {
              None,
              Fighter,
              Mage,
              Support
} player_class_type;

extern const char class_names[4][7];

void initialize_party(void);
#endif
