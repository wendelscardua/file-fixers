.include "../src/charmap.inc"
.segment "RODATA"
Eicar: .byte "Eicar"
Crypt: .byte "Crypt"
Buggy: .byte "Buggy"
Stega: .byte "Stega"
Nefet: .byte "Nefet"
.export _enemy_name
_enemy_name: .word Eicar, Crypt, Buggy, Stega, Nefet
.segment "BANK1"
.export _enemy_difficulty
_enemy_difficulty: .byte 1, 3, 4, 8, 16
.export _enemy_base_level
_enemy_base_level: .byte 1, 2, 4, 8, 12
.export _enemy_speed
_enemy_speed: .byte 6, 12, 12, 12, 18
.export _enemy_moves
_enemy_moves: .byte 1, 2, 2, 2, 3
.export _enemy_attack
_enemy_attack: .byte 33, 49, 49, 34, 50
.export _enemy_sprite_index
_enemy_sprite_index: .byte 0, 4, 24, 8, 28
