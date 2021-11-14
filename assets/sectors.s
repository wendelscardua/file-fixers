.segment "RODATA"

.repeat 8, sector_index
  .ident(.concat("sector_0", .string(sector_index))): .incbin .concat("sectors/sector-0",.string(sector_index),".bin")
.endrepeat

.repeat 8, sector_index
  .ident(.concat("sector_room_0", .string(sector_index))): .incbin .concat("sectors/sector-0",.string(sector_index),"-room.bin")
.endrepeat

.export _sector_metatiles
_sector_metatiles:
.repeat 8, sector_index
  .word .ident(.concat("sector_0", .string(sector_index)))
.endrepeat

.export _sector_rooms
_sector_rooms:
.repeat 8, sector_index
  .word .ident(.concat("sector_room_0", .string(sector_index)))
.endrepeat

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
