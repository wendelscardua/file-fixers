#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "lib/subrand.h"
#include "directions.h"
#include "dungeon.h"
#include "enemies.h"
#include "entities.h"
#include "irq_buffer.h"
#include "temp.h"
#include "wram.h"
#include "../assets/enemy-stats.h"
#include "../assets/sprites.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")

unsigned char num_entities, num_enemies;
unsigned char entity_aux;
unsigned char temp_w, temp_h;
unsigned char menu_cursor_row, menu_cursor_col;
unsigned char *room_ptr;
unsigned char current_entity_skill;
unsigned char skill_target_row, skill_target_col;
unsigned char skill_target_entity;

unsigned char turn_counter;

#pragma bss-name(pop)

unsigned char entity_row[MAX_ENTITIES];
unsigned char entity_col[MAX_ENTITIES];
direction entity_direction[MAX_ENTITIES];
unsigned char entity_turn_counter[MAX_ENTITIES];
unsigned char current_entity;
unsigned char current_entity_moves;
entity_state_enum current_entity_state;
unsigned char entity_x, entity_y;

void error() {
  // TODO: error handling?
  multi_vram_buffer_horz("ERROR", 5, NTADR_A(12, 12));
}

void refresh_moves_hud() {
  temp = current_entity_moves;
  temp_x = 0;
  while (temp >= 10) {
    temp -= 10;
    ++temp_x;
  }
  if (temp_x > 0) {
    one_vram_buffer(0x10 + temp_x, NTADR_A(22, 27));
  } else {
    one_vram_buffer(0x00, NTADR_A(22, 27));
  }
  one_vram_buffer(0x10 + temp, NTADR_A(23, 27));
}

void refresh_hud() {
  refresh_moves_hud();

  if (entity_type[current_entity] == Player) {
    multi_vram_buffer_horz((char *) player_name[current_entity], 5, NTADR_A(3, 25));
  } else {
    multi_vram_buffer_horz((char *) enemy_name[entity_type[current_entity]], 5, NTADR_A(3, 25));
  }
}

void init_entities(unsigned char stairs_row, unsigned char stairs_col) {
  turn_counter = 0;

  for(num_entities = 0; num_entities < 4; num_entities++) {
    entity_direction[num_entities] = Down;
    entity_turn_counter[num_entities] = subrand8(12);
  }
  entity_col[0] = entity_col[2] = stairs_col;
  entity_row[1] = entity_row[3] = stairs_row;
  entity_row[0] = stairs_row - 1;
  entity_row[2] = stairs_row + 1;
  entity_col[1] = stairs_col - 1;
  entity_col[3] = stairs_col + 1;

  num_enemies = 0;

  while (num_enemies == 0) {
    room_ptr = current_sector_room_data;
    while((temp_x = *room_ptr) != 0xff) {
      ++room_ptr;
      temp_y = *room_ptr;
      ++room_ptr;
      temp_w = *room_ptr;
      ++room_ptr;
      temp_h = *room_ptr;
      ++room_ptr;

      if (subrand8(1)) { // 50% chance to spawn in room
        spawn_enemy(num_entities);
        entity_col[num_entities] = temp_x + subrand8(temp_w);
        entity_row[num_entities] = temp_y + subrand8(temp_h);
        num_entities++;
        num_enemies++;
      }
    }
  }

  current_entity = num_entities - 1;
  next_entity();
}

void entity_handler() {
  if (num_entities == 0) return;
  switch(current_entity_state) {
  case EntityInput:
    entity_input_handler();
    break;
  case EntityMovement:
    entity_movement_handler();
    break;
  case EntityMenu:
    entity_menu_handler();
    break;
  case EntityPlayAction:
    entity_action_handler();
    break;
  }
}

extern unsigned char * current_sector;
unsigned char entity_collides() {
  if (temp_x == 0xff || temp_y == 0xff || temp_x == 12 || temp_y == 10) return 1;

  if (current_sector[temp_y * 12 + temp_x] == NullMetatile) return 1;

  for(i = 0; i < num_entities; i++) {
    if (entity_hp[i] > 0 && entity_col[i] == temp_x && entity_row[i] == temp_y) return 1;
  }

  return 0;
}

unsigned char set_melee_skill_target() {
  skill_target_row = entity_row[current_entity];
  skill_target_col = entity_col[current_entity];
  switch (entity_direction[current_entity]) {
  case Up: --skill_target_row; break;
  case Down: ++skill_target_row; break;
  case Left: --skill_target_col; break;
  case Right: ++skill_target_col; break;
  default:
    return 0;
  }

  if (skill_target_row > 9 || skill_target_col > 11) return 0;

  for(skill_target_entity = 0; skill_target_entity < num_entities; skill_target_entity++) {
    if ((entity_hp[skill_target_entity] > 0) &&
        (entity_row[skill_target_entity] == skill_target_row) &&
        (entity_col[skill_target_entity] == skill_target_col)) return 1;
  }

  return 0;
}

unsigned char enemy_lock_on_melee_target() {
  temp = entity_direction[current_entity];
  for(i = 0; i < 4; i++) {
    if (set_melee_skill_target() && skill_target_entity < 4) return 1;

    temp++;
    if (temp >= 4) temp = 0;
    entity_direction[current_entity] = temp;
  }
  return 0;
}

void enemy_random_walk() {
  temp_x = entity_col[current_entity];
  temp_y = entity_row[current_entity];
  temp = entity_direction[current_entity] = subrand8(3);
  switch(temp) {
  case Up: --temp_y; break;
  case Down: ++temp_y; break;
  case Left: --temp_x; break;
  case Right: ++temp_x; break;
  }

  if (!entity_collides()) {
    entity_row[current_entity] = temp_y;
    entity_col[current_entity] = temp_x;
    --current_entity_moves;
    refresh_moves_hud();
    current_entity_state = EntityMovement;
    entity_aux = 0x10;
  } else {
    ++entity_aux;
  }
}

void entity_input_handler() {
  switch(entity_type[current_entity]) {
  case Player:
    pad_poll(0);
    pad1_new = get_pad_new(0);
    if (pad1_new & (PAD_UP|PAD_DOWN|PAD_LEFT|PAD_RIGHT)) {
      temp_x = entity_col[current_entity];
      temp_y = entity_row[current_entity];
      if (pad1_new & PAD_UP) { --temp_y; temp = entity_direction[current_entity] = Up; }
      if (pad1_new & PAD_DOWN) { ++temp_y; temp = entity_direction[current_entity] = Down; }
      if (pad1_new & PAD_LEFT) { --temp_x; temp = entity_direction[current_entity] = Left; }
      if (pad1_new & PAD_RIGHT) { ++temp_x; temp = entity_direction[current_entity] = Right; }

      if (current_entity_moves > 0 && !entity_collides()) {
        entity_row[current_entity] = temp_y;
        entity_col[current_entity] = temp_x;
        --current_entity_moves;
        refresh_moves_hud();
        current_entity_state = EntityMovement;
        entity_aux = 0x10;
      }
    } else if (pad1_new & PAD_A) {
      current_entity_state = EntityMenu;
      menu_cursor_row = 0;
      menu_cursor_col = 0;
    } else if (pad1_new & PAD_B) { // Pass
      next_entity();
    }
    break;
  default:
    // Random walk
    if (enemy_lock_on_melee_target()) {
      entity_aux = 0;
      current_entity_skill = SkAttack;
      current_entity_state = EntityPlayAction;
    } else if (current_entity_moves > 0 && entity_aux < 0x10) {
      enemy_random_walk();
    } else {
      next_entity();
    }
    break;
  }
}

void entity_movement_handler() {
  --entity_aux;
  switch (entity_direction[current_entity]) {
  case Up: --entity_y; break;
  case Down: ++entity_y; break;
  case Left: --entity_x; break;
  case Right: ++entity_x; break;
  }
  if (entity_aux == 0) {
    current_entity_state = EntityInput;

    if (!sector_locked && entity_row[current_entity] == sector_down_row && entity_col[current_entity] == sector_down_column) {
      oam_clear();
      load_dungeon_sector(current_sector_index + 1);
      init_entities(sector_up_row, sector_up_column);
    } else if (entity_row[current_entity] == sector_up_row && entity_col[current_entity] == sector_up_column) {
      oam_clear();
      if (current_sector_index == 0) {
        // TODO: return to OS
      } else {
        load_dungeon_sector(current_sector_index - 1);
        init_entities(sector_down_row, sector_down_column);
      }
    }
  }
}

#define MENU_SCANLINE 0xc8
void entity_menu_handler() {
  double_buffer[double_buffer_index++] = MENU_SCANLINE - 1;
  double_buffer[double_buffer_index++] = 0xf6;
  double_buffer[double_buffer_index++] = 8;
  temp_int = 0x28 * current_entity;
  double_buffer[double_buffer_index++] = temp_int;
  double_buffer[double_buffer_index++] = 0;
  double_buffer[double_buffer_index++] = ((temp_int & 0xF8) << 2);

  pad_poll(0);
  pad1_new = get_pad_new(0);
  if (pad1_new & PAD_UP) {
    if (menu_cursor_row > 0) --menu_cursor_row;
  } else if (pad1_new & PAD_DOWN) {
    if (menu_cursor_row < 2) ++menu_cursor_row;
  } else if (pad1_new & PAD_LEFT) {
    if (menu_cursor_col > 0) --menu_cursor_col;
  } else if (pad1_new & PAD_RIGHT) {
    if (menu_cursor_col < 2) ++menu_cursor_col;
  } else if (pad1_new & PAD_B) {
    entity_aux = 0;
    current_entity_state = EntityInput;
  } else if (pad1_new & PAD_A) {
    switch (current_entity_skill = player_skills[current_entity][menu_cursor_col * 3 + menu_cursor_row]) {
    case SkNone:
      entity_aux = 0;
      current_entity_state = EntityInput;
      break;
    case SkAttack:
      entity_aux = 0;
      if (set_melee_skill_target()) {
        current_entity_state = EntityPlayAction;
      } else {
        next_entity();
      }
      break;
    case SkItem:
      // TODO: item
      break;
    case SkPass:
      next_entity();
      break;
    }
  }
}

unsigned char melee_to_hit() {
  signed char to_hit_bonus = 1;

  // TODO: str / dex bonus

  // + level
  to_hit_bonus += entity_lv[current_entity];

  // +1 if level <= 2
  if (entity_lv[current_entity] <= 2) ++to_hit_bonus;

  // TODO: AC
  to_hit_bonus += 7; // default AC = 7


  return (to_hit_bonus > subrand8(19) + 1);
}

const unsigned int xp_per_level[] =
  {
   0, // 0
   0, // 1
   20, // 2
   30, // 3
   50, // 4
   100, // 5
   200, // 6
   400, // 7
   800, // 8
   1600, // 9
   3200, // 10
   3600, // 11
   4000, // 12
   5000, // 13
   6000, // 14
   7000, // 15
   9000, // 16
   11000, // 17
   13000, // 18
   15000, // 19
   17000, // 20
   20000, // 21
   23000, // 22
   26000, // 23
   29000, // 24
   32000, // 25
   32000, // 26
   32000, // 27
   32000, // 28
   32000, // 29
   32000 // 30
  };

void gain_exp() {
  unsigned int exp, temp_exp, temp_goal;
  if (current_entity >= 4 || skill_target_entity < 4) return;

  exp = entity_lv[skill_target_entity];
  // ML * ML + 1
  exp = exp * exp + 1;
  if (entity_speed[skill_target_entity] >= 13) exp += 3;
  if (entity_speed[skill_target_entity] >= 19) exp += 2;
  if (entity_lv[skill_target_entity] >= 9) exp += 50;
  // TODO: bonus per monster attack:
  exp += 5;

  for(i = 0; i < 4; i++) {
    if (entity_hp[i] == 0) continue;
    temp = entity_lv[i];
    if (temp >= 30) continue;
    // current player gets 100% xp, others get 50%
    if (i == current_entity) {
      temp_exp = exp;
    } else {
      temp_exp = exp / 2;
    }

    temp_goal = xp_per_level[temp];
    if (temp_exp + player_xp[i] >= temp_goal) {
      player_xp[i] = player_xp[i] + temp_exp - temp_goal;
      // TODO: maybe check if they can gain multiple levels
      ++entity_lv[i];

      // TODO: maybe per class?
      temp = subrand8(7) + 1;
      player_max_hp[i] += temp;
      entity_hp[i] += temp;
      if (entity_hp[i] > player_max_hp[i]) entity_hp[i] = player_max_hp[i];
    } else {
      player_xp[i] += temp_exp;
    }
  }

  num_enemies--;

  if (num_enemies == 0 && sector_locked) {
    unlock_sector();
  }
}

unsigned char roll_dice(unsigned char dice_spec) {
  unsigned char amount, sides, total;
  amount = dice_spec & 0b111;
  sides = (dice_spec >> 3) - 1;
  total = amount;
  while(amount > 0) {
    --amount;
    total += subrand8(sides);
  }
  return total;
}

#define BASIC_SKILL_ANIM_LEN 0x18

void entity_action_handler() {
  switch(current_entity_skill) {
  case SkAttack:
    entity_aux++;
    if (entity_aux >= BASIC_SKILL_ANIM_LEN) {
      if (melee_to_hit()) {
        temp = roll_dice(entity_attack[current_entity]);
        if (entity_hp[skill_target_entity] <= temp) {
          entity_hp[skill_target_entity] = 0;
          gain_exp();
        } else {
          entity_hp[skill_target_entity] -= temp;
        }
      }
      entity_aux = 0;
      next_entity();
    }
    break;
  default:
    error();
    break;
  }
}

void regen() {
  if ((turn_counter & 0x111) == 0 &&
      entity_hp[current_entity] < player_max_hp[current_entity]) {
    if (entity_lv[current_entity] >= 10) {
      // TODO: base on constitution?
      temp = subrand8(16) + 1;
      if (temp > entity_lv[current_entity] - 9) {
        temp = entity_lv[current_entity] - 9;
      }
      entity_hp[current_entity] += temp;
      if (entity_hp[current_entity] > player_max_hp[current_entity]) {
        entity_hp[current_entity] = player_max_hp[current_entity];
      }
    } else {
      entity_hp[current_entity]++;
    }
  }
}

void next_entity() {
  if (num_entities == 0) return;

  while(1) {
    ++current_entity;
    if (current_entity >= num_entities) {
      current_entity = 0;
      ++turn_counter;
    }
    if (entity_hp[current_entity] == 0) continue;

    entity_turn_counter[current_entity] += entity_speed[current_entity];
    if (entity_turn_counter[current_entity] >= NORMAL_SPEED) {
      entity_turn_counter[current_entity] -= NORMAL_SPEED;
      current_entity_moves = entity_moves[current_entity];
      current_entity_state = EntityInput;
      entity_aux = 0;

      entity_x = entity_col[current_entity] * 0x10 + 0x20;
      entity_y = entity_row[current_entity] * 0x10 + 0x20 - 1;

      if (current_entity < 4) regen();

      refresh_hud();
      break;
    }
  }
}

void draw_entities() {
  if (current_entity_state == EntityPlayAction) {
    switch(current_entity_skill) {
    case SkAttack:
      temp = entity_direction[current_entity];
      temp_x = skill_target_col * 0x10 + 0x20;
      temp_y = skill_target_row * 0x10 + 0x20 - 1;
      oam_meta_spr(temp_x, temp_y, melee_sprite[temp]);
      break;
    }
  }

  for(i = 0; i < num_entities; ++i) {
    if (entity_hp[i] == 0) continue;
    if (i == current_entity && current_entity_state == EntityMovement) {
      switch(entity_type[i]) {
      case Player:
        switch(entity_direction[i]) {
        case Up: temp = (PLAYER_STEP_1_UP_SPR << 2) | i; break;
        case Down: temp = (PLAYER_STEP_1_DOWN_SPR << 2) | i; break;
        case Left: temp = (PLAYER_STEP_1_LEFT_SPR << 2) | i; break;
        case Right: temp = (PLAYER_STEP_1_RIGHT_SPR << 2) | i; break;
        }
        if (entity_aux & 0b1000) {
          temp += (1 << 2);
        }
        oam_meta_spr(entity_x, entity_y, player_sprite[temp]);
        break;
      default: // enemies
        switch(entity_direction[i]) {
        case Up: temp = ENEMY_LEFT_1_SPR; break;
        case Down: temp = ENEMY_RIGHT_1_SPR; break;
        case Left: temp = ENEMY_LEFT_1_SPR; break;
        case Right: temp = ENEMY_RIGHT_1_SPR; break;
        }
        if (entity_aux & 0b1000) {
          temp++;
        }
        oam_meta_spr(entity_x,
                     entity_y,
                     enemy_sprite[
                                  enemy_sprite_index[entity_type[i]] | temp
                                  ]);
      }
    } else {
      temp_x = entity_col[i] * 0x10 + 0x20;
      temp_y = entity_row[i] * 0x10 + 0x20 - 1;
      switch(entity_type[i]) {
      case Player:
        switch(entity_direction[i]) {
        case Up: temp = (PLAYER_UP_SPR << 2) | i; break;
        case Down: temp = (PLAYER_DOWN_SPR << 2) | i; break;
        case Left: temp = (PLAYER_LEFT_SPR << 2) | i; break;
        case Right: temp = (PLAYER_RIGHT_SPR << 2) | i; break;
        }
        oam_meta_spr(temp_x, temp_y, player_sprite[temp]);

        break;
      default: // enemies
        switch(entity_direction[i]) {
        case Up: temp = ENEMY_LEFT_2_SPR; break;
        case Down: temp = ENEMY_RIGHT_2_SPR; break;
        case Left: temp = ENEMY_LEFT_1_SPR; break;
        case Right: temp = ENEMY_RIGHT_1_SPR; break;
        }
        oam_meta_spr(temp_x,
                     temp_y,
                     enemy_sprite[
                                  enemy_sprite_index[entity_type[i]] | temp
                                  ]);
      }
    }
  }

  if (current_entity_state == EntityMenu) {
    temp_x = 0x40 * menu_cursor_col + 0x08;
    temp_y = 0x08 * menu_cursor_row + 0xc4;
    if ((get_frame_count() & 0b11000) == 0b11000) {
      temp_x -= 0x04;
    }
    oam_meta_spr(temp_x, temp_y, menu_cursor_sprite);
  }
}
