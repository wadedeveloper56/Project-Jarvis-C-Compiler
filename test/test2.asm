.x64p
option casemap : none

.data?
m SDWORD ? ; global var m type = int

.data
x SDWORD 3; global var x type = int value = 3

.code
_add PROC a:SDWORD,b:SDWORD
LOCAL _c:SDWORD
movsxd rax, a ; load a
push rax
movsxd rax, b ; load b
push rax
mov rax, 2
push rax
pop rbx
pop rax
imul rax, rbx
push rax
pop rbx
pop rax
add rax, rbx
push rax

    ; global ref c
  ret
_add ENDP

_main PROC 
LOCAL _y:SDWORD
mov rax, 4
push rax
; global ref x
call _add
add esp, 16
push rax

    ; global ref y
  ret
_main ENDP

end
