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
_add PROC a:SDWORD,b:SDWORD
  LOCAL _c:SDWORD
  movsxd rax, a ;load and sign extend 'a' in to register 1
  push rax ;push 4 on to stack
  movsxd rax, b ;load and sign extend 'b' in to register 1
  push rax ;push 4 on to stack
  mov rax, 2 ;load immediate into register
  push rax ;push register A on to the stack
  pop rbx ;pop rhs into register B
  pop rax ;pop lhs into register A
  imul rax, rbx ;multiply registers A and B and store result in A
  push rax ;push register A on to the stack
  pop rbx ;pop rhs into register B
  pop rax ;pop lhs into register A
  add rax, rbx ;add registers A and B and store result in A
  push rax ;push register A on to the stack
  mov _c, eax ;var decl c type = int
  mov eax, _c ;load local 'c' in to register A
  ret ;return 1
_add ENDP

;-----------------
; params:
; locals:
;   y : int
;-----------------
_main PROC 
  LOCAL _y:SDWORD
  mov rax, 4 ;load immediate into register
  push rax ;push register A on to the stack
  mov eax, x ;load global 'x' in to register A
  push rax ;push register A on to the stack
  call _add ;call function 'add'
  push rax ;push register A on to the stack
  mov _y, eax ;var decl y type = int
  mov eax, _y ;load local 'y' in to register A
  ret ;return 1
_main ENDP

end
