.386
option segment:use16
.model small, c;
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
  mov eax, a			;load 'a' in to register A
  push eax			;push register A on to stack
  mov eax, b			;load 'b' in to register A
  push eax			;push register A on to stack
  mov eax, 5		;load immediate into register
  push eax			;push register A on to the stack
  pop ebx			;pop top of stack into register B
  pop eax			;pop top of stack into register A
  imul eax, ebx		;multiply registers A and B and store result in A
  push eax			;push register EAX on to the stack
  pop ebx			;pop top of stack into register B
  pop eax			;pop top of stack into register A
  add eax, ebx		;add registers A and B and store result in A
  push eax			;push register EAX on to the stack
  mov _c, eax		;move result of binary expression from register A in to c
;----------Handle Function Body Statements-----------
  mov eax, _c			;load 'c' in to register A
  push eax			;push register A on to stack
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
  mov eax, a			;load 'a' in to register A
  push eax			;push register A on to stack
  mov eax, b			;load 'b' in to register A
  push eax			;push register A on to stack
  mov eax, 3		;load immediate into register
  push eax			;push register A on to the stack
  pop ebx			;pop top of stack into register B
  pop eax			;pop top of stack into register A
  cdq 				;signed 32-bit value in the EAX register and sign-extends it into the 64-bit pair EDX:EAX
  idiv ebx			;divides the 64-bit value in EDX:EAX by EBX - quotient is stored in EAX -remainder is stored in EDX
  push eax			;push register EAX on to the stack
  pop ebx			;pop top of stack into register B
  pop eax			;pop top of stack into register A
  add eax, ebx		;add registers A and B and store result in A
  push eax			;push register EAX on to the stack
  mov _c, eax		;move result of binary expression from register A in to c
;----------Handle Function Body Statements-----------
  mov eax, _c			;load 'c' in to register A
  push eax			;push register A on to stack
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
  mov _y, eax			;move result of invoke from register EAX to variable 'y'
  push eax				;push the result in register A onto stack
  invoke _func2, m, 6	;invoke function 'func2' with 2 arguments
  mov _x, eax			;move result of invoke from register EAX to variable 'x'
  push eax				;push the result in register A onto stack
;----------Handle Function Body Statements-----------
  mov eax, _x			;load 'x' in to register A
  push eax			;push register A on to stack
  mov eax, _y			;load 'y' in to register A
  push eax			;push register A on to stack
  pop bx				;pop top of stack into register B
  pop ax				;pop top of stack into register A
  add ax, bx			;add registers A and B and store result in A
  ret 					;return from function
;----------------------------------------------------
_func3 ENDP

end
