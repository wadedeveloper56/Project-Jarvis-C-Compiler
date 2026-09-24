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
  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  imul ax, bx		;multiply registers A and B and store result in A
  push ax			;push register A on to the stack
  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  add ax, bx		;add registers A and B and store result in A
  push ax			;push register A on to the stack
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
  mov eax, a			;load 'a' in to register A
  push eax			;push register A on to stack
  mov eax, b			;load 'b' in to register A
  push eax			;push register A on to stack
  mov eax, 3		;load immediate into register
  push eax			;push register A on to the stack
  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  cdq 				;extend eax to edx:eax for idiv
  idiv bx			;integer divide registers A and B and store result in A
  push ax			;push register A on to the stack
  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  add ax, bx		;add registers A and B and store result in A
  push ax			;push register A on to the stack
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
  pop bx				;pop top of stack into register B
  pop ax				;pop top of stack into register A
  add ax, bx			;add registers A and B and store result in A
  ret 					;return from function
;----------------------------------------------------
_func3 ENDP

end
