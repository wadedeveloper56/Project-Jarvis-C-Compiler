.x64p
option casemap : none

.data?
m SDWORD ? ;global var m type = int

.data
x SDWORD 3 ;global var x type = int

.code
;-----------------
; params:
;   a : int
;   b : int
; locals:
;   c : int
;-----------------
_func1 PROC _a:SDWORD,_b:SDWORD
  LOCAL _c:SDWORD
;----------- Local variable initialization ----------
  movsxd rax, _a		;load and sign extend 'a' in to register A
  push rax			;push register A on to stack

  movsxd rax, _b		;load and sign extend 'b' in to register A
  push rax			;push register A on to stack

  mov eax, 5		;load immediate into register
  push rax			;push register A on to the stack

  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  imul eax, ebx		;multiply registers A and B and store result in A
  push rax			;push register A on to the stack

  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  add eax, ebx		;add registers A and B and store result in A
  push rax			;push register A on to the stack

  mov _c, eax		;move result of binary expression from register A to variable 'c'
;----------Handle Function Body Statements-----------
