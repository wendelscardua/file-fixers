.segment "RODATA"
.feature force_range

.export _default_cursor_sprite
.export _clicking_cursor_sprite
.export _loading_cursor_sprite

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
