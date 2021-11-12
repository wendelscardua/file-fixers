.import _current_entity, _entity_speed, _entity_turn_counter

.segment "CODE"

.export _entity_turn
.proc _entity_turn
  LDX _current_entity
  LDA _entity_turn_counter, X
  CLC
  ADC _entity_speed, X
  STA _entity_turn_counter, X
  BCC :+
  LDA #1
  RTS
:
  LDA #0
  RTS
.endproc
