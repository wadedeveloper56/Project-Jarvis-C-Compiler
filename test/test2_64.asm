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
  imul eax, ebx			;multiply registers A and B and store result in A

  push rax			;push register A on to the stack

  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  add eax, ebx			;add registers A and B and store result in A

  push rax			;push register A on to the stack

  mov _c, eax		;move result of binary expression from register A to variable 'c'
;----------Handle Function Body Statements-----------
  movsxd rax, _c		;load and sign extend 'c' in to register A
  push rax			;push register A on to stack

  ret 					;return from function
;----------------------------------------------------
_func1 ENDP

;-----------------
; params:
;   a : int
;   b : int
; locals:
;   c : int
;-----------------
_func2 PROC _a:SDWORD,_b:SDWORD
  LOCAL _c:SDWORD
;----------- Local variable initialization ----------
  movsxd rax, _a		;load and sign extend 'a' in to register A
  push rax			;push register A on to stack

  movsxd rax, _b		;load and sign extend 'b' in to register A
  push rax			;push register A on to stack

  mov eax, 3		;load immediate into register
  push rax			;push register A on to the stack

  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  cdq 				;extend eax to edx:eax for idiv
  idiv ebx				;integer divide registers A and B and store result in A
  push rax			;push register A on to the stack

  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  add eax, ebx			;add registers A and B and store result in A

  push rax			;push register A on to the stack

  mov _c, eax		;move result of binary expression from register A to variable 'c'
;----------Handle Function Body Statements-----------
  movsxd rax, _c		;load and sign extend 'c' in to register A
  push rax			;push register A on to stack

  ret 					;return from function
;----------------------------------------------------
_func2 ENDP

;-----------------
; params:
; locals:
;   y : int
;   x : int
;-----------------
_func3 PROC 
  LOCAL _y:SDWORD,_x:SDWORD
;----------- Local variable initialization ----------
  invoke _func1, x, 4	;invoke function 'func1' with 2 arguments
  mov _y, eax			;move result of invoke from register A to variable 'y'
  push rax			;push the result in register A onto stack
  invoke _func2, m, 6	;invoke function 'func2' with 2 arguments
  mov _x, eax			;move result of invoke from register A to variable 'x'
  push rax			;push the result in register A onto stack
;----------Handle Function Body Statements-----------
  movsxd rax, _x		;load and sign extend 'x' in to register A
  push rax			;push register A on to stack

  movsxd rax, _y		;load and sign extend 'y' in to register A
  push rax			;push register A on to stack

  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  add eax, ebx			;add registers A and B and store result in A

  ret 					;return from function
;----------------------------------------------------
_func3 ENDP

end
