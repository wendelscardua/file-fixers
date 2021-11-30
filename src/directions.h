#ifndef _DIRECTIONS_H_
#define _DIRECTIONS_H_

typedef enum {
              Up,
              Down,
              Left,
              Right
} direction;

#define NUDGE(direction)                                \
  switch((direction)) {                                 \
  case Up: --temp_y; break;                             \
  case Down: ++temp_y; break;                           \
  case Left: --temp_x; break;                           \
  case Right: ++temp_x; break;                          \
  }


#endif
