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
  imul eax, ebx		;multiply registers A and B and store result in A
  push rax			;push register RAX on to the stack
  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  add eax, ebx		;add registers A and B and store result in A
  push rax			;push register RAX on to the stack
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
  cdq 				;signed 32-bit value in the EAX register and sign-extends it into the 64-bit pair EDX:EAX
  idiv ebx			;divides the 64-bit value in EDX:EAX by EBX - quotient is stored in EAX -remainder is stored in EDX
  push rax			;push register RAX on to the stack
  pop rbx			;pop top of stack into register B
  pop rax			;pop top of stack into register A
  add eax, ebx		;add registers A and B and store result in A
  push rax			;push register RAX on to the stack
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
  mov _y, eax			;move result of invoke from register RAX to variable 'y'
  invoke _func2, m, 6	;invoke function 'func2' with 2 arguments
  mov _x, eax			;move result of invoke from register RAX to variable 'x'
;----------Handle Function Body Statements-----------
  movsxd rax, _x		;load and sign extend 'x' in to register A
  push rax			;push register A on to stack
  movsxd rax, _y		;load and sign extend 'y' in to register A
  push rax			;push register A on to stack
  pop rbx				;pop top of stack into register B
  pop rax				;pop top of stack into register A
  add rax, rbx			;add registers A and B and store result in A
  ret 					;return from function
;----------------------------------------------------
_func3 ENDP

end
