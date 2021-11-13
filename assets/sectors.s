.segment "RODATA"

sector_00: .incbin "sectors/sector-00.bin"
sector_01: .incbin "sectors/sector-01.bin"
sector_02: .incbin "sectors/sector-02.bin"
sector_03: .incbin "sectors/sector-03.bin"
sector_04: .incbin "sectors/sector-04.bin"
sector_05: .incbin "sectors/sector-05.bin"
sector_06: .incbin "sectors/sector-06.bin"
sector_07: .incbin "sectors/sector-07.bin"

.export _sector_metatiles
_sector_metatiles:
.word sector_00, sector_01, sector_02, sector_03
.word sector_04, sector_05, sector_06, sector_07

; metatiles: null, ground, down, up, locked
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

.include "../src/charmap.inc"

.export _dungeon_name
_dungeon_name:
gpu_dll:  .byte "GPU.DLL "
disk_dll: .byte "Disk.DLL"
io_dll:   .byte "IO.DLL  "
ram_dll:  .byte "RAM.DLL "
;_dungeon_name: .word gpu_dll, disk_dll, io_dll, ram_dll
