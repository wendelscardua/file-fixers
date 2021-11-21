.include "../src/constants.inc"
.include "../src/enemies.inc"

.segment "RODATA"
.feature force_range

.export _default_cursor_sprite
.export _clicking_cursor_sprite
.export _loading_cursor_sprite
.export _menu_cursor_sprite
.export _melee_sprite
.export _player_sprite
.export _enemy_sprite

_default_cursor_sprite:
.byte   0,  0,$00,0
.byte   8,  0,$01,0
.byte   0,  8,$10,0
.byte   8,  8,$11,0
.byte   0, 16,$20,0
.byte   8, 16,$21,0
.byte $80

_clicking_cursor_sprite:
.byte   0,-24,$02,0
.byte   8,-24,$03,0
.byte   0,-16,$12,0
.byte   8,-16,$13,0
.byte   0,- 8,$22,0
.byte   8,- 8,$23,0
.byte $80

_loading_cursor_sprite:
.byte - 8,- 8,$04,0
.byte   0,- 8,$05,0
.byte - 8,  0,$14,0
.byte   0,  0,$15,0
.byte $80

_menu_cursor_sprite:
.byte   0,  0,$0c,0
.byte   8,  0,$0d,0
.byte   0,  8,$1c,0
.byte   8,  8,$1d,0
.byte $80

melee_right_sprite:
.byte   4,  0,$0e,1
.byte   4,  8,$1e,1
.byte $80

melee_left_sprite:
.byte   4,  0,$0e,1|OAM_FLIP_H
.byte   4,  8,$1e,1|OAM_FLIP_H
.byte $80

melee_down_sprite:
.byte   0,  4,$1f,1
.byte   8,  4,$0f,1
.byte $80

melee_up_sprite:
.byte   0,  4,$1f,1|OAM_FLIP_V
.byte   8,  4,$0f,1|OAM_FLIP_V
.byte $80

_melee_sprite: .word melee_up_sprite, melee_down_sprite, melee_left_sprite, melee_right_sprite

.repeat 4, palette
  .ident (.concat("enemy_left_0_p", .string(palette))):
  .byte   4,  7,$80,palette
  .byte $80

  .ident (.concat("enemy_left_alt_0_p", .string(palette))):
  .byte   4,  7,$90,palette
  .byte $80

  .ident (.concat("enemy_right_0_p", .string(palette))):
  .byte   4,  7,$80,palette
  .byte $80

  .ident (.concat("enemy_right_alt_0_p", .string(palette))):
  .byte   4,  7,$90,palette
  .byte $80

  .ident (.concat("enemy_left_1_p", .string(palette))):
  .byte   4,  0,$81,palette
  .byte   4,  8,$91,palette
  .byte $80

  .ident (.concat("enemy_left_alt_1_p", .string(palette))):
  .byte   4,  0,$81,palette
  .byte   4,  8,$92,palette
  .byte $80

  .ident (.concat("enemy_right_1_p", .string(palette))):
  .byte   4,  0,$81,palette
  .byte   4,  8,$91,palette
  .byte $80

  .ident (.concat("enemy_right_alt_1_p", .string(palette))):
  .byte   4,  0,$81,palette
  .byte   4,  8,$92,palette
  .byte $80

  .ident (.concat("enemy_left_2_p", .string(palette))):
  .byte   1,  0,$83,palette
  .byte   6,  8,$93,palette
  .byte $80

  .ident (.concat("enemy_left_alt_2_p", .string(palette))):
  .byte   1,  0,$83,palette
  .byte   6,  8,$82,palette
  .byte $80

  .ident (.concat("enemy_right_2_p", .string(palette))):
  .byte   7,  0,$83,palette|OAM_FLIP_H
  .byte   2,  8,$93,palette|OAM_FLIP_H
  .byte $80

  .ident (.concat("enemy_right_alt_2_p", .string(palette))):
  .byte   7,  0,$83,palette|OAM_FLIP_H
  .byte   2,  8,$82,palette|OAM_FLIP_H
  .byte $80
.endrepeat

_enemy_sprite:
.repeat 4, palette
  .repeat NUM_ENEMY_MODELS, enemy_index
    .word .ident (.concat("enemy_left_", .string(enemy_index), "_p", .string(palette)))
    .word .ident (.concat("enemy_left_alt_", .string(enemy_index), "_p", .string(palette)))
    .word .ident (.concat("enemy_right_", .string(enemy_index), "_p", .string(palette)))
    .word .ident (.concat("enemy_right_alt_", .string(enemy_index), "_p", .string(palette)))
  .endrepeat
.endrepeat

.repeat 4, player_index
  .ident (.concat("player_facing_up_", .string(player_index))):

	.byte   0,  0,$02 + $20 * player_index,1
	.byte   8,  0,$03 + $20 * player_index,1
	.byte   0,  8,$12 + $20 * player_index,1
	.byte   8,  8,$13 + $20 * player_index,1

	.byte $80


  .ident (.concat("player_facing_down_", .string(player_index))):

	.byte   0,  0,$00 + $20 * player_index,1
	.byte   8,  0,$01 + $20 * player_index,1
	.byte   0,  8,$10 + $20 * player_index,1
	.byte   8,  8,$11 + $20 * player_index,1

	.byte $80

  .ident (.concat("player_facing_left_", .string(player_index))):

	.byte   0,  0,$04 + $20 * player_index,1
	.byte   8,  0,$05 + $20 * player_index,1
	.byte   0,  8,$14 + $20 * player_index,1
	.byte   8,  8,$15 + $20 * player_index,1

	.byte $80


  .ident (.concat("player_facing_right_", .string(player_index))):

	.byte   8,  0,$04 + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  0,$05 + $20 * player_index,1|OAM_FLIP_H
	.byte   8,  8,$14 + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  8,$15 + $20 * player_index,1|OAM_FLIP_H

	.byte $80

  .ident (.concat("player_step_1_up_", .string(player_index))):

	.byte   0,  0,$08 + $20 * player_index,1
	.byte   8,  0,$09 + $20 * player_index,1
	.byte   0,  8,$18 + $20 * player_index,1
	.byte   8,  8,$19 + $20 * player_index,1

	.byte $80

  .ident (.concat("player_step_2_up_", .string(player_index))):

	.byte   8,  0,$08 + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  0,$09 + $20 * player_index,1|OAM_FLIP_H
	.byte   8,  8,$18 + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  8,$19 + $20 * player_index,1|OAM_FLIP_H

	.byte $80

  .ident (.concat("player_step_1_down_", .string(player_index))):

  .byte   0,  0,$06 + $20 * player_index,1
	.byte   8,  0,$07 + $20 * player_index,1
	.byte   0,  8,$16 + $20 * player_index,1
	.byte   8,  8,$17 + $20 * player_index,1

	.byte $80

  .ident (.concat("player_step_2_down_", .string(player_index))):

	.byte   8,  0,$06 + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  0,$07 + $20 * player_index,1|OAM_FLIP_H
	.byte   8,  8,$16 + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  8,$17 + $20 * player_index,1|OAM_FLIP_H

	.byte $80

  .ident (.concat("player_step_1_left_", .string(player_index))):

	.byte   0,  0,$04 + $20 * player_index,1
	.byte   8,  0,$05 + $20 * player_index,1
	.byte   0,  8,$14 + $20 * player_index,1
	.byte   8,  8,$15 + $20 * player_index,1

	.byte $80

  .ident (.concat("player_step_2_left_", .string(player_index))):

	.byte   0,  0,$0a + $20 * player_index,1
	.byte   8,  0,$0b + $20 * player_index,1
	.byte   0,  8,$1a + $20 * player_index,1
	.byte   8,  8,$1b + $20 * player_index,1

	.byte $80

  .ident (.concat("player_step_1_right_", .string(player_index))):

	.byte   8,  0,$04 + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  0,$05 + $20 * player_index,1|OAM_FLIP_H
	.byte   8,  8,$14 + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  8,$15 + $20 * player_index,1|OAM_FLIP_H

	.byte $80

  .ident (.concat("player_step_2_right_", .string(player_index))):

	.byte   8,  0,$0a + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  0,$0b + $20 * player_index,1|OAM_FLIP_H
	.byte   8,  8,$1a + $20 * player_index,1|OAM_FLIP_H
	.byte   0,  8,$1b + $20 * player_index,1|OAM_FLIP_H

	.byte $80
.endrepeat

_player_sprite:
.repeat 4, player_index
  .word .ident (.concat("player_facing_up_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_facing_down_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_facing_left_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_facing_right_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_step_1_up_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_step_2_up_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_step_1_down_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_step_2_down_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_step_1_left_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_step_2_left_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_step_1_right_", .string(player_index)))
.endrepeat
.repeat 4, player_index
  .word .ident (.concat("player_step_2_right_", .string(player_index)))
.endrepeat
