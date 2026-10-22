.x64p
option casemap : none



.data
_x SDWORD 5
.data?
_u SDWORD  ?
_m SDWORD  ?

.code
_func1 PROTO C _a:SDWORD , _b:SDWORD ;
_func2 PROTO C _a:SDWORD , _b:SDWORD ;
_main PROTO C;

_func1 PROC C, _a:SDWORD , _b:SDWORD 
	LOCAL _c:SDWORD
	movsxd rax, DWORD PTR _a
	push rax
	movsxd rax, DWORD PTR _b
	push rax
	mov rax, 5
	mov rbx, rax
	pop rax
	imul rax, rbx
	mov rbx, rax
	pop rax
	add rax, rbx
	mov DWORD PTR _c, eax
	movsxd rax, DWORD PTR _c
	movsxd rax, DWORD PTR _c
	ret
_func1 endp

_func2 PROC C, _a:SDWORD , _b:SDWORD 
	LOCAL _c:SDWORD
	movsxd rax, DWORD PTR _a
	push rax
	movsxd rax, DWORD PTR _b
	push rax
	mov rax, 3
	mov rbx, rax
	pop rax
	cqo
	idiv rbx
	mov rbx, rax
	pop rax
	add rax, rbx
	mov DWORD PTR _c, eax
	movsxd rax, DWORD PTR _c
	movsxd rax, DWORD PTR _c
	ret
_func2 endp

_main PROC C
	LOCAL _a:SDWORD, _b:SDWORD
	movsxd rax, DWORD PTR _a
	push rax
	movsxd rax, DWORD PTR _b
	mov rbx, rax
	pop rax
	add rax, rbx
	ret
_main endp
end

