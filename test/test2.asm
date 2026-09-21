.x64p
option casemap : none

.data?
m SDWORD ? ; global var m type = int

.data
x SDWORD 3; global var x type = int value = 3

.code
_add PROC a:SDWORD,b:SDWORD
LOCAL _c:SDWORD
movsxd rax, a ; load a1
push rax; push 4
movsxd rax, b ; load b1
push rax; push 4
mov rax, 2
push rax; push 2
pop rbx
pop rax
imul rax, rbx
push rax; push 1
pop rbx
pop rax
add rax, rbx
push rax; push 1
; var decl c type = int
mov _c, eax ; store local 1c
ret ; return 1
_add ENDP

_main PROC 
LOCAL _y:SDWORD
mov rax, 4
push rax; push 2
; global ref x2
call _add
add esp, 16
push rax; push 3
; var decl y type = int
mov _y, eax ; store local 1y
ret ; return 1
_main ENDP

end
