.segment "RODATA"

.export _bg_palette, _dungeon_bg_palette, _sprites_palette

_bg_palette: .incbin "bg.pal"
_dungeon_bg_palette: .incbin "bg-dungeon.pal"
_sprites_palette: .incbin "sprites.pal"
