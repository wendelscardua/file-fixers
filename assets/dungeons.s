.segment "RODATA"

dungeon_00: .incbin "dungeons/dungeon-00.bin"
dungeon_01: .incbin "dungeons/dungeon-01.bin"
dungeon_02: .incbin "dungeons/dungeon-02.bin"
dungeon_03: .incbin "dungeons/dungeon-03.bin"
dungeon_04: .incbin "dungeons/dungeon-04.bin"
dungeon_05: .incbin "dungeons/dungeon-05.bin"
dungeon_06: .incbin "dungeons/dungeon-06.bin"
dungeon_07: .incbin "dungeons/dungeon-07.bin"

.export _sector_metatiles
_sector_metatiles:
.word dungeon_00, dungeon_01, dungeon_02, dungeon_03
.word dungeon_04, dungeon_05, dungeon_06, dungeon_07

.export _metatile_UL_tiles
_metatile_UL_tiles:  .byte $00, $c0, $c2, $c4, $c6

.export _metatile_UR_tiles
_metatile_UR_tiles:  .byte $00, $c1, $c3, $c5, $c7

.export _metatile_DL_tiles
_metatile_DL_tiles:  .byte $00, $d0, $d2, $d4, $d6

.export _metatile_DR_tiles
_metatile_DR_tiles:  .byte $00, $d1, $d3, $d5, $d7

.export _metatile_attribute
_metatile_attribute: .byte $00, $00, $00, $00, $00
