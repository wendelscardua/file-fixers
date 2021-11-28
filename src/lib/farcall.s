.segment "STARTUP"

.proc _farcall
  .export _farcall
  .import pusha, popa, callptr4
  .import _set_prg_8000
  .importzp BP_BANK_8000, tmp4

  ;; store the current bank on the stack, switch to new bank

  ; push the current prg bank on the stack
  lda BP_BANK_8000
  jsr pusha

  ; select the new bank
  lda tmp4
  jsr _set_prg_8000

  ;; jump to wrapped call
  jsr callptr4

  ;; restore the previous prg bank and pop it

  jsr popa
  jsr _set_prg_8000

  rts
.endproc

.proc _farcallax
  .export _farcallax
  .import pusha, popa, callptr4
  .import _set_prg_8000
  .importzp BP_BANK_8000, tmp4

  pha
  txa
  pha

  ;; store the current bank on the stack, switch to new bank

  ; push the current prg bank on the stack
  lda BP_BANK_8000
  jsr pusha

  ; select the new bank
  lda tmp4
  jsr _set_prg_8000

  ;; jump to wrapped call
  pla
  tax
  pla
  jsr callptr4
  pha
  txa
  pha

  ;; restore the previous prg bank and pop it

  jsr popa
  jsr _set_prg_8000

  pla
  tax
  pla
  rts
.endproc
