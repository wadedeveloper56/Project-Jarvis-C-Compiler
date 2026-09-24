.x64p
option casemap : none

.data?
m SDWORD ? ;global var m type = int

.data
x SDWORD 3 ;global var x type = int value = 3

.code
;-----------------
; params:
;   a : int
;   b : int
; locals:
;   c : int
;-----------------
_func1 PROC a:SDWORD,b:SDWORD
  LOCAL _c:SDWORD
;----------- Local variable initialization ----------
  movsxd rax, a		;load and sign extend 'a' in to register A
  push rax			;push register A on to stack
  movsxd rax, b		;load and sign extend 'b' in to register A
  push rax			;push register A on to stack
  mov rax, 5		;load immediate into register
  push rax			;push register A on to the stack
  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  imul rax, rbx		;multiply registers A and B and store result in A
  push rax			;push register A on to the stack
  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  add rax, rbx		;add registers A and B and store result in A
  push rax			;push register A on to the stack
  mov _c, eax		;move result of binary expression from register A in to c
;----------Handle Function Body Statements-----------
  mov eax, _c			;load local 'c' in to register A
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
_func2 PROC a:SDWORD,b:SDWORD
  LOCAL _c:SDWORD
;----------- Local variable initialization ----------
  movsxd rax, a		;load and sign extend 'a' in to register A
  push rax			;push register A on to stack
  movsxd rax, b		;load and sign extend 'b' in to register A
  push rax			;push register A on to stack
  mov rax, 3		;load immediate into register
  push rax			;push register A on to the stack
  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  cdq 				;extend eax to edx:eax for idiv
  idiv rbx			;integer divide registers A and B and store result in A
  push rax			;push register A on to the stack
  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  add rax, rbx		;add registers A and B and store result in A
  push rax			;push register A on to the stack
  mov _c, eax		;move result of binary expression from register A in to c
;----------Handle Function Body Statements-----------
  mov eax, _c			;load local 'c' in to register A
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
  push eax				;push the result in register A onto stack
  invoke _func2, m, 6	;invoke function 'func2' with 2 arguments
  mov _x, eax			;move result of invoke from register A to variable 'x'
  push eax				;push the result in register A onto stack
;----------Handle Function Body Statements-----------
  pop rbx				;pop top of stack into register B
  pop rax				;pop top of stack into register A
  add rax, rbx			;add registers A and B and store result in A
  ret 					;return from function
;----------------------------------------------------
_func3 ENDP

end
