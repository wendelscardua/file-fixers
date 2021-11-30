#ifndef _PLAYERS_H_
#define _PLAYERS_H_
typedef enum {
              None,
              Fighter,
              Mage,
              Support,
              NumClasses
} player_class_type;

extern const char class_names[4][7];
extern const unsigned char moves_per_class_and_level[3][30];

void initialize_party(void);
#endif
