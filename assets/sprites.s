.include "../src/constants.inc"

.segment "RODATA"
.feature force_range

.export _default_cursor_sprite
.export _clicking_cursor_sprite
.export _loading_cursor_sprite
.export _player_sprite

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

.repeat 4, player_index
  .ident (.concat("player_facing_up_", .string(player_index))):

	.byte   0,  0,$02 + $20 * player_index,0
	.byte   8,  0,$03 + $20 * player_index,0
	.byte   0,  8,$12 + $20 * player_index,0
	.byte   8,  8,$13 + $20 * player_index,0

	.byte $80


  .ident (.concat("player_facing_down_", .string(player_index))):

	.byte   0,  0,$00 + $20 * player_index,0
	.byte   8,  0,$01 + $20 * player_index,0
	.byte   0,  8,$10 + $20 * player_index,0
	.byte   8,  8,$11 + $20 * player_index,0

	.byte $80

  .ident (.concat("player_facing_left_", .string(player_index))):

	.byte   0,  0,$04 + $20 * player_index,0
	.byte   8,  0,$05 + $20 * player_index,0
	.byte   0,  8,$14 + $20 * player_index,0
	.byte   8,  8,$15 + $20 * player_index,0

	.byte $80


  .ident (.concat("player_facing_right_", .string(player_index))):

	.byte   8,  0,$04 + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  0,$05 + $20 * player_index,0|OAM_FLIP_H
	.byte   8,  8,$14 + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  8,$15 + $20 * player_index,0|OAM_FLIP_H

	.byte $80

  .ident (.concat("player_step_1_up_", .string(player_index))):

	.byte   0,  0,$08 + $20 * player_index,0
	.byte   8,  0,$09 + $20 * player_index,0
	.byte   0,  8,$18 + $20 * player_index,0
	.byte   8,  8,$19 + $20 * player_index,0

	.byte $80

  .ident (.concat("player_step_2_up_", .string(player_index))):

	.byte   8,  0,$08 + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  0,$09 + $20 * player_index,0|OAM_FLIP_H
	.byte   8,  8,$18 + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  8,$19 + $20 * player_index,0|OAM_FLIP_H

	.byte $80

  .ident (.concat("player_step_1_down_", .string(player_index))):

  .byte   0,  0,$06 + $20 * player_index,0
	.byte   8,  0,$07 + $20 * player_index,0
	.byte   0,  8,$16 + $20 * player_index,0
	.byte   8,  8,$17 + $20 * player_index,0

	.byte $80

  .ident (.concat("player_step_2_down_", .string(player_index))):

	.byte   8,  0,$06 + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  0,$07 + $20 * player_index,0|OAM_FLIP_H
	.byte   8,  8,$16 + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  8,$17 + $20 * player_index,0|OAM_FLIP_H

	.byte $80

  .ident (.concat("player_step_1_left_", .string(player_index))):

	.byte   0,  0,$04 + $20 * player_index,0
	.byte   8,  0,$05 + $20 * player_index,0
	.byte   0,  8,$14 + $20 * player_index,0
	.byte   8,  8,$15 + $20 * player_index,0

	.byte $80

  .ident (.concat("player_step_2_left_", .string(player_index))):

	.byte   0,  0,$0a + $20 * player_index,0
	.byte   8,  0,$0b + $20 * player_index,0
	.byte   0,  8,$1a + $20 * player_index,0
	.byte   8,  8,$1b + $20 * player_index,0

	.byte $80

  .ident (.concat("player_step_1_right_", .string(player_index))):

	.byte   8,  0,$04 + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  0,$05 + $20 * player_index,0|OAM_FLIP_H
	.byte   8,  8,$14 + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  8,$15 + $20 * player_index,0|OAM_FLIP_H

	.byte $80

  .ident (.concat("player_step_2_right_", .string(player_index))):

	.byte   8,  0,$0a + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  0,$0b + $20 * player_index,0|OAM_FLIP_H
	.byte   8,  8,$1a + $20 * player_index,0|OAM_FLIP_H
	.byte   0,  8,$1b + $20 * player_index,0|OAM_FLIP_H

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
