#include "lib/neslib.h"
#include "castle.h"
#include "../assets/sprites.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

void castle_handler() {
}

void draw_castle_sprites() {
  oam_meta_spr(0x70, 0x3e, amda_sprite);
  oam_meta_spr(0x80, 0x3e, intelle_sprite);
}
