#include "lib/nesdoug.h"
#include "lib/neslib.h"
#include "lib/subrand.h"
#include "mmc3/mmc3_code.h"
#include "charmap.h"
#include "dice.h"
#include "directions.h"
#include "dungeon.h"
#include "enemies.h"
#include "entities.h"
#include "irq_buffer.h"
#include "main.h"
#include "skills.h"
#include "temp.h"
#include "wram.h"
#include "../assets/enemy-stats.h"
#include "../assets/sprites.h"

#pragma code-name ("CODE")
#pragma rodata-name ("RODATA")

#pragma bss-name(push, "ZEROPAGE")

unsigned char num_entities, num_enemies, num_players;
unsigned char entity_aux;
unsigned char temp_w, temp_h;
unsigned char menu_cursor_row, menu_cursor_col, menu_cursor_index;
unsigned char *room_ptr;

unsigned char current_entity;

unsigned char entity_sprite_index;

unsigned char turn_counter;

#pragma bss-name(pop)

unsigned char entity_row[MAX_ENTITIES];
unsigned char entity_col[MAX_ENTITIES];
direction entity_direction[MAX_ENTITIES];
unsigned char entity_turn_counter[MAX_ENTITIES];
unsigned char current_entity_moves;
entity_state_enum current_entity_state;
unsigned char entity_x, entity_y;

// indexed by current level
const unsigned int xp_per_level[] =
  {
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

#ifdef DEBUG
void error() {
  // TODO: error handling?
  multi_vram_buffer_horz("ERROR", 5, NTADR_A(12, 12));
  while(1);
}
#endif

void refresh_skills_hud() {
  // input: i = player index
  temp_x = 3;
  temp_y = 0;
  for(temp = 0; temp < 9; temp++) {
    temp_attr = player_skills[i][temp];
    if (temp_attr != SkNone) {
      temp_bank = change_prg_8000(2);
      multi_vram_buffer_horz(skill_name[temp_attr], 8, NTADR_C((temp_x), (temp_y + 5 * i)));
      set_prg_8000(temp_bank);
    }
    temp_y++;
    if (temp_y == 3) {
      temp_y = 0; temp_x += 9;
      ppu_wait_nmi();
      clear_vram_buffer();
    }
  }
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

void refresh_gauge(unsigned char row) {
  // temp_int_x = value
  // temp_int_y = max_value
  for(i = 0; i < 8; i++) {
    if (temp_int_y == 0) {
      one_vram_buffer(row == 0 ? 0x71 : 0x81, NTADR_A(6 + i, 26 + row));
    } else {
      temp_int = (i + 1) * temp_int_y / 8;
      if (temp_int_x >= temp_int) {
        one_vram_buffer(row == 0 ? 0x75 : 0x85, NTADR_A(6 + i, 26 + row));
      } else {
        temp_int = i * temp_int_y / 8;
        if (temp_int_x <= temp_int) {
          one_vram_buffer(row == 0 ? 0x71 : 0x81, NTADR_A(6 + i, 26 + row));
        } else {
          temp = 32 * (temp_int_x - temp_int) / temp_int_y;
          one_vram_buffer((row == 0 ? 0x71 : 0x81) + temp,
                          NTADR_A(6 + i, 26 + row));
        }
      }
    }
  }
}

void refresh_hp_sp_hud() {
  temp_int_x = entity_hp[current_entity];
  temp_int_y = entity_max_hp[current_entity];
  refresh_gauge(0);

  if (current_entity < 4) {
    temp_int_x = player_sp[current_entity];
    temp_int_y = player_max_sp[current_entity];
  } else {
    temp_int_x = 0;
    temp_int_y = 0;
  }
  refresh_gauge(1);
}

void write_xp() {
  char xp_str_buffer[5];

  xp_str_buffer[0] = 0x10;
  while (temp_int >= 10000u) {
    temp_int -= 10000u;
    xp_str_buffer[0]++;
  }
  xp_str_buffer[1] = 0x10;
  while (temp_int >= 1000u) {
    temp_int -= 1000u;
    xp_str_buffer[1]++;
  }
  xp_str_buffer[2] = 0x10;
  while (temp_int >= 100u) {
    temp_int -= 100u;
    xp_str_buffer[2]++;
  }
  xp_str_buffer[3] = 0x10;
  while (temp_int >= 10u) {
    temp_int -= 10u;
    xp_str_buffer[3]++;
  }
  xp_str_buffer[4] = 0x10 + temp_int;
  multi_vram_buffer_horz(xp_str_buffer, 5, NTADR_A(19, temp_y));
}

void refresh_xp_hud() {
  temp_y = 25;
  temp_int = player_xp[current_entity];
  write_xp();
  temp_y = 26;
  temp_int = xp_per_level[entity_lv[current_entity]];
  write_xp();
}

void refresh_hud() {
  refresh_moves_hud();
  refresh_hp_sp_hud();

  temp = entity_lv[current_entity];
  one_vram_buffer(0x10 + (temp / 10), NTADR_A(12, 25));
  one_vram_buffer(0x10 + (temp % 10), NTADR_A(13, 25));

  if (entity_type[current_entity] == Player) {
    multi_vram_buffer_horz((char *) player_name[current_entity], 5, NTADR_A(3, 25));
    refresh_xp_hud();
  } else {
    multi_vram_buffer_horz((char *) enemy_name[entity_type[current_entity]], 5, NTADR_A(3, 25));
    multi_vram_buffer_horz("     ", 5, NTADR_A(19, 25));
    multi_vram_buffer_horz("     ", 5, NTADR_A(19, 26));
  }
}

void init_entities(unsigned char stairs_row, unsigned char stairs_col) {
  turn_counter = 0;
  num_enemies = 0;
  num_players = 0;

  for(num_entities = 0; num_entities < 4; num_entities++) {
    if (entity_hp[num_entities] == 0) continue;

    entity_direction[num_entities] = Down;
    entity_turn_counter[num_entities] = subrand8(12);
    num_players++;

    i = num_entities;
#ifdef DEBUG
    temp_bank = change_prg_8000(2);
    temp = player_class[i];
    for(temp_x = 0; temp_x < 6; temp_x++) {
      player_skills[i][3 + temp_x] = skills_per_class[temp - 1][temp_x];
    }
    set_prg_8000(temp_bank);
#endif
    refresh_skills_hud();
  }
  entity_col[0] = entity_col[2] = stairs_col;
  entity_row[1] = entity_row[3] = stairs_row;
  entity_row[0] = stairs_row - 1;
  entity_row[2] = stairs_row + 1;
  entity_col[1] = stairs_col - 1;
  entity_col[3] = stairs_col + 1;

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
    if (num_players == 0) go_to_game_over();
    break;
  case EntityMovement:
    entity_movement_handler();
    break;
  case EntityMenu:
    entity_menu_handler();
    break;
  case EntityAskTarget:
    entity_ask_target_handler();
    break;
  case EntityPlayAction:
    entity_action_handler();
    break;
  }
}

extern unsigned char * current_sector;

unsigned char collides_with_map() {
  if (temp_x == 0xff || temp_y == 0xff || temp_x == 12 || temp_y == 10) return 1;
  if (current_sector[temp_y * 12 + temp_x] == NullMetatile) return 1;
  return 0;
}

unsigned char entity_collides() {
  if (collides_with_map()) return 1;

  for(i = 0; i < num_entities; i++) {
    if (entity_hp[i] > 0 && entity_col[i] == temp_x && entity_row[i] == temp_y) return 1;
  }

  return 0;
}

unsigned char enemy_lock_on_melee_target() {
  unsigned char direction_counter;
  temp_attr = entity_status[current_entity] & STATUS_CONFUSE;
  for(direction_counter = 0; direction_counter < 4; direction_counter++) {
    skill_target_direction = entity_direction[current_entity];
    skill_target_row = entity_row[current_entity];
    skill_target_col = entity_col[current_entity];
    if (set_melee_skill_target() &&
        ((skill_target_entity < 4) == (temp_attr == 0))) {
      return 1;
    }
    if (entity_direction[current_entity] == 3) entity_direction[current_entity] = 0;
    else entity_direction[current_entity]++;
  }
  return 0;
}

void enemy_random_walk() {
  temp_x = entity_col[current_entity];
  temp_y = entity_row[current_entity];
  temp = entity_direction[current_entity] = subrand8(3);
  NUDGE(temp);

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
      if (entity_status[current_entity] & STATUS_CONFUSE) { pad1_new = rand8(); }
      temp_x = entity_col[current_entity];
      temp_y = entity_row[current_entity];
      if (pad1_new & PAD_UP) { --temp_y; temp = Up; }
      else if (pad1_new & PAD_DOWN) { ++temp_y; temp = Down; }
      else if (pad1_new & PAD_LEFT) { --temp_x; temp = Left; }
      else if (pad1_new & PAD_RIGHT) { ++temp_x; temp = Right; }
      entity_direction[current_entity] = temp;

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
      menu_cursor_index = 0;
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

    if (current_entity >= 4) return;

    if (!sector_locked && entity_row[current_entity] == sector_down_row && entity_col[current_entity] == sector_down_column) {
      oam_clear();
      load_dungeon_sector(current_sector_index + 1);
      init_entities(sector_up_row, sector_up_column);
    } else if (entity_row[current_entity] == sector_up_row && entity_col[current_entity] == sector_up_column) {
      oam_clear();
      if (current_sector_index == 0) {
        check_dungeon_completion();
        return_from_dungeon();
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
    if (menu_cursor_row > 0) { --menu_cursor_row; --menu_cursor_index; }
  } else if (pad1_new & PAD_DOWN) {
    if (menu_cursor_row < 2) { ++menu_cursor_row; ++menu_cursor_index; }
  } else if (pad1_new & PAD_LEFT) {
    if (menu_cursor_col > 0) { --menu_cursor_col; menu_cursor_index -= 3; }
  } else if (pad1_new & PAD_RIGHT) {
    if (menu_cursor_col < 2) { ++menu_cursor_col; menu_cursor_index += 3; }
  } else if (pad1_new & PAD_B) {
    entity_aux = 0;
    current_entity_state = EntityInput;
  } else if (pad1_new & PAD_A) {
    switch (current_entity_skill = player_skills[current_entity][menu_cursor_index]) {
    case SkNone:
      entity_aux = 0;
      current_entity_state = EntityInput;
      break;
    case SkAttack:
      entity_aux = 0;
      skill_target_row = entity_row[current_entity];
      skill_target_col = entity_col[current_entity];
      skill_target_direction = entity_direction[current_entity];
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
    default:
      if (!have_enough_sp()) { break; }
      skill_target_row = entity_row[current_entity];
      skill_target_col = entity_col[current_entity];
      skill_target_direction = entity_direction[current_entity];
      if (skill_is_targeted()) {
        current_entity_state = EntityAskTarget;
        break;
      } else if (skill_can_hit()) {
        consume_sp();
        refresh_hp_sp_hud();
        entity_aux = 0;
        current_entity_state = EntityPlayAction;
      }
      break;
    }
  }
}

void entity_ask_target_handler() {
  pad_poll(0);
  pad1_new = get_pad_new(0);
  if (pad1_new == 0) return;

  if (pad1_new & PAD_UP) {
    if (skill_target_row > 0) { --skill_target_row; }
  } else if (pad1_new & PAD_DOWN) {
    if (skill_target_row < 9) { ++skill_target_row; }
  } else if (pad1_new & PAD_LEFT) {
    if (skill_target_col > 0) { --skill_target_col; }
  } else if (pad1_new & PAD_RIGHT) {
    if (skill_target_col < 11) { ++skill_target_col; }
  } else if (pad1_new & PAD_B) {
    entity_aux = 0;
    current_entity_state = EntityInput;
  } else if (pad1_new & PAD_A) {
    temp_x = skill_target_col;
    temp_y = skill_target_row;
    if (collides_with_map()
        || (skill_target_col == sector_down_column && skill_target_row == sector_down_row)
        || (skill_target_col == sector_up_column && skill_target_row == sector_up_row)) return;
    skill_target_entity = find_entity();
    if ((skill_target_entity == 0xff) == (current_entity_skill != SkTele)) {
      entity_aux = 0;
      current_entity_state = EntityInput;
      return;
    }
    consume_sp();
    refresh_hp_sp_hud();
    entity_aux = 0;
    current_entity_state = EntityPlayAction;
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
  if (skill_target_entity < 4) {
    // player AC
    to_hit_bonus += 7;
  } else {
    to_hit_bonus += 10;
  }

  if (entity_status[skill_target_entity] & STATUS_PROTECT) {
    to_hit_bonus -= 5;
  }

  return (to_hit_bonus > roll_die(20));
}

unsigned char ray_to_hit() {
  signed char to_hit_bonus = 0;

  // TODO: skill / dex bonus
  to_hit_bonus += 2; // skilled
  to_hit_bonus += 2; // dex

  // TODO: AC
  if (skill_target_entity < 4) {
    // player AC
    to_hit_bonus += 7;
  } else {
    to_hit_bonus += 10;
  }

  if (entity_status[skill_target_entity] & STATUS_PROTECT) {
    to_hit_bonus -= 5;
  }

  return (to_hit_bonus + roll_die(20) > 4);
}

unsigned char fire_to_hit() {
  signed char to_hit_bonus = 10;

  // TODO: AC
  if (skill_target_entity < 4) {
    // player AC
    to_hit_bonus += 7;
  } else {
    to_hit_bonus += 10;
  }

  if (entity_status[skill_target_entity] & STATUS_PROTECT) {
    to_hit_bonus -= 5;
  }

  return (to_hit_bonus > roll_die(20));
}

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

#ifdef DEBUG
  exp += 500;
#endif

  for(i = 0; i < 4; i++) {
    if (entity_hp[i] == 0) continue;
    temp = entity_lv[i];
    if (temp >= 30) continue;
    // current player gets 150% xp, others get 100%
    if (i == current_entity) {
      temp_exp = exp + exp / 2;
    } else {
      temp_exp = exp;
    }

    temp_goal = xp_per_level[temp];
    if (temp_exp + player_xp[i] >= temp_goal) {
      player_xp[i] = player_xp[i] + temp_exp - temp_goal;
      // TODO: maybe check if they can gain multiple levels
      ++entity_lv[i];
      if (entity_lv[i] > party_level) party_level = entity_lv[i];


      // increase HP

      // TODO: maybe per class?
      temp = roll_die(8);
      entity_max_hp[i] += temp;
      entity_hp[i] += temp;

      switch(player_class[i]) {
      case Fighter:
        if (entity_lv[i] <= 10) temp = 1;
        else temp = 2;
        break;
      case Mage:
        if (entity_lv[i] <= 12) temp = 2;
        else temp = 3;
        break;
      case Support:
        if (entity_lv[i] <= 20) temp = 1;
        else temp = 2;
        break;
      default:
        // TODO: error
        break;
      }

      // increase SP

      // TODO: maybe add wiz
      temp = 2 + subrand8(16 / 2 + temp - 1); // wiz~16

      switch(player_class[i]) {
      case Fighter:
        temp = temp * 3 / 2;
        break;
      case Mage:
        temp = temp * 2;
        break;
      case Support:
        temp = temp * 3 / 2;
        break;
      default:
        // TODO: error
        break;
      }
      if (player_max_sp[i] > 255 - temp) {
        temp = 255 - player_max_sp[i];
      }
      player_sp[i] += temp;
      player_max_sp[i] += temp;

      // gain skill
      temp = entity_lv[i];
      temp_y = LEVEL_FOR_SKILL;
      for(temp_x = 0; temp_x < 6; temp_x++, temp_y += LEVEL_FOR_SKILL) {
        if (temp == temp_y) {
          temp_bank = change_prg_8000(2);
          player_skills[i][3 + temp_x] = skills_per_class[player_class[i] - 1][temp_x];
          set_prg_8000(temp_bank);
          refresh_skills_hud();
          break;
        }
      }

      // gain moves
      entity_moves[i] = moves_per_class_and_level[player_class[i] - 1][entity_lv[i] - 1];
    } else {
      player_xp[i] += temp_exp;
    }
  }
}

void skill_damage(unsigned char damage) {
  if (entity_hp[skill_target_entity] <= damage) {
    entity_hp[skill_target_entity] = 0;
    if (skill_target_entity < 4) num_players--;
    else {
      num_enemies--;
      if (num_enemies == 0 && sector_locked) {
        unlock_sector();
      }
    }
    gain_exp();
  } else {
    entity_hp[skill_target_entity] -= damage;
  }
}

#define BASIC_SKILL_ANIM_LEN 0x18

void entity_action_handler() {
  entity_aux++;
  if (entity_aux >= BASIC_SKILL_ANIM_LEN) {
    switch(current_entity_skill) {
    case SkAttack:
      if (melee_to_hit()) {
        skill_damage(roll_dice(entity_attack[current_entity].amount, entity_attack[current_entity].sides));
      }
      break;
    case SkBolt:
      if (ray_to_hit()) {
        skill_damage(roll_dice(entity_lv[current_entity] / 2 + 1, 6));
      }

      // keep going
      // TODO wall reflection?
      if (set_forward_skill_target()) {
        entity_aux = 0;
        return;
      }

      break;
    case SkConfuse:
      entity_status[skill_target_entity] |= STATUS_CONFUSE;
      entity_status_turns[skill_target_entity] = STATUS_LENGTH;
      entity_direction[skill_target_entity] = subrand8(3);
      break;
    case SkFire:
      if (fire_to_hit()) {
        skill_damage(roll_dice(2, 12));
      }
      break;
    case SkFreeze:
      entity_status[skill_target_entity] |= STATUS_FREEZE;
      entity_status_turns[skill_target_entity] = STATUS_LENGTH;
      skill_damage(roll_dice(1, 4));
      break;
    case SkJoust:
      temp_x = entity_col[current_entity];
      temp_y = entity_row[current_entity];
      NUDGE(entity_direction[current_entity]);
      if (find_entity() != 0xff) {
        entity_col[skill_target_entity] = entity_col[current_entity];
        entity_row[skill_target_entity] = entity_row[current_entity];
        entity_col[current_entity] = temp_x;
        entity_row[current_entity] = temp_y;
        if (melee_to_hit()) {
          skill_damage(roll_dice(2 * entity_attack[current_entity].amount, entity_attack[current_entity].sides));
        }
      } else {
        entity_col[current_entity] = temp_x;
        entity_row[current_entity] = temp_y;
        entity_aux = 0;
        return;
      }
      break;
    case SkHaste:
      entity_status[skill_target_entity] |= STATUS_HASTE;
      entity_status_turns[skill_target_entity] = STATUS_LENGTH;
      break;
    case SkHeal:
      // TODO: maybe add chance to fumble
      entity_hp[skill_target_entity] += roll_dice(6, 4);
      if (entity_hp[skill_target_entity] > entity_max_hp[skill_target_entity]) {
        entity_hp[skill_target_entity] = entity_max_hp[skill_target_entity];
      }
      break;
    case SkProtect:
    case SkShield:
      entity_status[skill_target_entity] |= STATUS_PROTECT;
      entity_status_turns[skill_target_entity] = STATUS_LENGTH;
      break;
    case SkSlash:
      if (melee_to_hit()) {
        skill_damage(roll_dice(2 * entity_attack[current_entity].amount, entity_attack[current_entity].sides));
      }
      break;
    case SkSlow:
      entity_status[skill_target_entity] |= STATUS_SLOW;
      entity_status_turns[skill_target_entity] = STATUS_LENGTH;
      break;
    case SkTele:
      entity_row[current_entity] = skill_target_row;
      entity_col[current_entity] = skill_target_col;
      break;
    case SkThrow:
      temp_x = skill_target_col;
      temp_y = skill_target_row;
      NUDGE(entity_direction[current_entity]);
      if (entity_collides()) {
        entity_row[skill_target_entity] = skill_target_row;
        entity_col[skill_target_entity] = skill_target_col;
        if (melee_to_hit()) {
          skill_damage(roll_dice(entity_attack[current_entity].amount, entity_attack[current_entity].sides));
        }
      } else {
        entity_row[skill_target_entity] = skill_target_row = temp_y;
        entity_col[skill_target_entity] = skill_target_col = temp_x;
        entity_aux = 0;
        return;
      }
      break;
    }
    entity_aux = 0;
    next_entity();
  }
}

void regen() {
  if ((turn_counter & 0x111) == 0 &&
      entity_hp[current_entity] < entity_max_hp[current_entity]) {
    if (entity_lv[current_entity] >= 10) {
      // TODO: base on constitution?
      temp = roll_die(16);
      if (temp > entity_lv[current_entity] - 9) {
        temp = entity_lv[current_entity] - 9;
      }
      entity_hp[current_entity] += temp;
      if (entity_hp[current_entity] > entity_max_hp[current_entity]) {
        entity_hp[current_entity] = entity_max_hp[current_entity];
      }
    } else {
      entity_hp[current_entity]++;
    }
  }
  // TODO: scale with level?
  if ((turn_counter & 0x111) == 0 &&
      player_sp[current_entity] < player_max_sp[current_entity]) {
    temp = roll_die(3); // scale with wiz + int ?
    player_sp[current_entity] += temp;
    if (player_sp[current_entity] > player_max_sp[current_entity]) {
      player_sp[current_entity] = player_max_sp[current_entity];
    }
  }
}

void next_entity() {
  if (num_entities == 0) return;

  while(1) {
    // check if current entity was fast enough to earn a second turn
    if (entity_turn_counter[current_entity] < NORMAL_SPEED || entity_hp[current_entity] == 0) {
      // skip to next entity
      ++current_entity;
      if (current_entity >= num_entities) {
        current_entity = 0;
        ++turn_counter;
      }
    }

    if (entity_hp[current_entity] == 0) continue;

    temp_attr = entity_status[current_entity];

    if (entity_turn_counter[current_entity] < NORMAL_SPEED) {
      temp = entity_speed[current_entity];

      if (temp_attr & STATUS_HASTE) {
        temp = temp * 2;
      }
      if (temp_attr & STATUS_SLOW) {
        temp = temp / 2;
      }

      entity_turn_counter[current_entity] += temp;
    }

    if (entity_turn_counter[current_entity] >= NORMAL_SPEED) {
      entity_turn_counter[current_entity] -= NORMAL_SPEED;
      if (temp_attr & STATUS_FREEZE) {
        current_entity_moves = 0;
      } else {
        current_entity_moves = entity_moves[current_entity];
      }
      current_entity_state = EntityInput;
      entity_aux = 0;

      // consume status timer
      if (entity_status_turns[current_entity] > 0) {
        entity_status_turns[current_entity]--;
      } else {
        entity_status[current_entity] = 0;
      }

      entity_x = entity_col[current_entity] * 0x10 + 0x20;
      entity_y = entity_row[current_entity] * 0x10 + 0x20 - 1;

      if (current_entity < 4) regen();

      refresh_hud();
      break;
    }
  }
}

unsigned char find_entity() {
  // finds an entity at coords temp_x, temp_y
  // return 0xff if not found
  for(i = 0; i < num_entities; i++) {
    if (entity_hp[i] > 0 && entity_row[i] == temp_y && entity_col[i] == temp_x) return i;
  }
  return 0xff;
}

void draw_entities() {
  // TODO: spell effects
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

  if (current_entity_state == EntityAskTarget) {
    temp_x = 0x10 * skill_target_col + 0x15;
    temp_y = 0x10 * skill_target_row + 0x20;
    if ((get_frame_count() & 0b11000) == 0b11000) {
      temp_x -= 0x04;
    }
    oam_meta_spr(temp_x, temp_y, menu_cursor_sprite);
  }

  entity_sprite_index = get_frame_count() & 0x0f;

  for(i = 0; i < MAX_ENTITIES; ++i, entity_sprite_index = (entity_sprite_index + 7) & 0x0f) {
    if (entity_sprite_index >= num_entities) continue;
    if (entity_hp[entity_sprite_index] == 0) continue;
    if (entity_sprite_index == current_entity && current_entity_state == EntityMovement) {
      switch(entity_type[entity_sprite_index]) {
      case Player:
        switch(entity_direction[entity_sprite_index]) {
        case Up: temp = (PLAYER_STEP_1_UP_SPR << 2) | entity_sprite_index; break;
        case Down: temp = (PLAYER_STEP_1_DOWN_SPR << 2) | entity_sprite_index; break;
        case Left: temp = (PLAYER_STEP_1_LEFT_SPR << 2) | entity_sprite_index; break;
        case Right: temp = (PLAYER_STEP_1_RIGHT_SPR << 2) | entity_sprite_index; break;
        }
        if (entity_aux & 0b1000) {
          temp += (1 << 2);
        }
        oam_meta_spr(entity_x, entity_y, player_sprite[temp]);
        break;
      default: // enemies
        switch(entity_direction[entity_sprite_index]) {
        case Up: temp = ENEMY_LEFT_1_SPR; break;
        case Down: temp = ENEMY_RIGHT_1_SPR; break;
        case Left: temp = ENEMY_LEFT_1_SPR; break;
        case Right: temp = ENEMY_RIGHT_1_SPR; break;
        }
        if (entity_aux & 0b1000) {
          temp++;
        }
        temp_bank = change_prg_8000(1);
        oam_meta_spr(entity_x,
                     entity_y,
                     enemy_sprite[
                                  enemy_sprite_index[entity_type[entity_sprite_index]] | temp
                                  ]);
        set_prg_8000(temp_bank);
      }
    } else {
      temp_x = entity_col[entity_sprite_index] * 0x10 + 0x20;
      temp_y = entity_row[entity_sprite_index] * 0x10 + 0x20 - 1;
      switch(entity_type[entity_sprite_index]) {
      case Player:
        switch(entity_direction[entity_sprite_index]) {
        case Up: temp = (PLAYER_UP_SPR << 2) | entity_sprite_index; break;
        case Down: temp = (PLAYER_DOWN_SPR << 2) | entity_sprite_index; break;
        case Left: temp = (PLAYER_LEFT_SPR << 2) | entity_sprite_index; break;
        case Right: temp = (PLAYER_RIGHT_SPR << 2) | entity_sprite_index; break;
        }
        oam_meta_spr(temp_x, temp_y, player_sprite[temp]);

        break;
      default: // enemies
        switch(entity_direction[entity_sprite_index]) {
        case Up: temp = ENEMY_LEFT_2_SPR; break;
        case Down: temp = ENEMY_RIGHT_2_SPR; break;
        case Left: temp = ENEMY_LEFT_1_SPR; break;
        case Right: temp = ENEMY_RIGHT_1_SPR; break;
        }
        temp_bank = change_prg_8000(1);
        oam_meta_spr(temp_x,
                     temp_y,
                     enemy_sprite[
                                  enemy_sprite_index[entity_type[entity_sprite_index]] | temp
                                  ]);
        set_prg_8000(temp_bank);
      }
    }
  }

  if (current_entity_state == EntityMenu) {
    temp_x = 0x48 * menu_cursor_col + 0x08;
    temp_y = 0x08 * menu_cursor_row + 0xc4;
    if ((get_frame_count() & 0b11000) == 0b11000) {
      temp_x -= 0x04;
    }
    oam_meta_spr(temp_x, temp_y, menu_cursor_sprite);
  }
}
