.386
option segment:use16
.model small, c;
option casemap : none

.data?
m SWORD ? ;global var m type = int

.data
x SWORD 3 ;global var x type = int

.code
;-----------------
; params:
;   a : int
;   b : int
; locals:
;   c : int
;-----------------
_func1 PROC _a:SWORD,_b:SWORD
  LOCAL _c:SWORD
;----------- Local variable initialization ----------
  mov ax, _a		;load parameter a
  push ax			;push register A on to stack

  mov ax, _b		;load parameter b
  push ax			;push register A on to stack

  mov ax, 5		;load immediate into register
  push ax			;push register A on to the stack

  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  imul ax, bx			;multiply registers A and B and store result in A

  push ax			;push register A on to the stack

  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  add ax, bx			;add registers A and B and store result in A

  push ax			;push register A on to the stack

  mov _c, ax		;move result of binary expression from register A to variable 'c'
;----------Handle Function Body Statements-----------
  mov ax, _c		;load parameter c
  push ax			;push register A on to stack

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
_func2 PROC _a:SWORD,_b:SWORD
  LOCAL _c:SWORD
;----------- Local variable initialization ----------
  mov ax, _a		;load parameter a
  push ax			;push register A on to stack

  mov ax, _b		;load parameter b
  push ax			;push register A on to stack

  mov ax, 3		;load immediate into register
  push ax			;push register A on to the stack

  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  cwd 				;extend eax to edx:eax for idiv
  idiv bx				;integer divide registers A and B and store result in A
  push ax			;push register A on to the stack

  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  add ax, bx			;add registers A and B and store result in A

  push ax			;push register A on to the stack

  mov _c, ax		;move result of binary expression from register A to variable 'c'
;----------Handle Function Body Statements-----------
  mov ax, _c		;load parameter c
  push ax			;push register A on to stack

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
  LOCAL _y:SWORD,_x:SWORD
;----------- Local variable initialization ----------
  invoke _func1, x, 4	;invoke function 'func1' with 2 arguments
  mov _y, ax			;move result of invoke from register A to variable 'y'
  push ax			;push the result in register A onto stack
  invoke _func2, m, 6	;invoke function 'func2' with 2 arguments
  mov _x, ax			;move result of invoke from register A to variable 'x'
  push ax			;push the result in register A onto stack
;----------Handle Function Body Statements-----------
  mov ax, _x		;load parameter x
  push ax			;push register A on to stack

  mov ax, _y		;load parameter y
  push ax			;push register A on to stack

  pop bx			;pop top of stack into register B
  pop ax			;pop top of stack into register A
  add ax, bx			;add registers A and B and store result in A

  ret 					;return from function
;----------------------------------------------------
_func3 ENDP

end
