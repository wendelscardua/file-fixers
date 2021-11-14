.segment "ZEROPAGE"
subrand_mask: .res 1
subrand_bound: .res 1

.segment "CODE"
.import _rand8

.export _subrand8
.proc _subrand8
  STA subrand_bound
  TAX
  LDA upper_bound_mask_lt, X
  STA subrand_mask
loop:
  JSR _rand8
  AND subrand_mask
  CMP subrand_bound
  BCC return
  BEQ return
  JMP loop
return:
  LDX #$00
.endproc

.segment "RODATA"

upper_bound_mask_lt:
.byte 0
.repeat 1
  .byte 1
.endrepeat
.repeat 2
  .byte 3
.endrepeat
.repeat 4
  .byte 7
.endrepeat
.repeat 8
  .byte 15
.endrepeat
.repeat 16
  .byte 31
.endrepeat
.repeat 32
  .byte 63
.endrepeat
.repeat 64
  .byte 127
.endrepeat
