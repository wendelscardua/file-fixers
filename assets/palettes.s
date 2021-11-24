.segment "RODATA"

.export _bg_palette, _castle_bg_palette, _dungeon_bg_palette
.export _sprites_palette, _castle_sprites_palette, _dungeon_sprites_palette

_bg_palette: .incbin "bg.pal"
_castle_bg_palette: .incbin "bg-castle.pal"
_dungeon_bg_palette: .incbin "bg-dungeon.pal"
_sprites_palette: .incbin "sprites.pal"
_castle_sprites_palette: .incbin "sprites-castle.pal"
_dungeon_sprites_palette: .incbin "sprites-dungeon.pal"
