.charmap '.', $0e
.charmap ' ', $00
.repeat $40, i
  .charmap $40 + i, $20 + i
.endrepeat
