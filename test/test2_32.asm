.386
.model flat, c;
option casemap : none



.data
_x SDWORD 5
.data?
_u SDWORD  ?
_m SDWORD  ?

.code
_func1 PROTO C _a:SDWORD , _b:SDWORD ;
_func2 PROTO C _a:SDWORD , _b:SDWORD ;
_func3 PROTO C;

_func1 PROC C, _a:SDWORD , _b:SDWORD 
	LOCAL _c:SDWORD
	mov eax, DWORD PTR _a
	push eax
	mov eax, DWORD PTR _b
	push eax
	mov eax, 5
	mov ebx, eax
	pop eax
	imul eax, ebx
	mov ebx, eax
	pop eax
	add eax, ebx
	mov DWORD PTR _c, eax
	mov eax, DWORD PTR _c
	mov eax, DWORD PTR _c
	ret
_func1 endp

_func2 PROC C, _a:SDWORD , _b:SDWORD 
	LOCAL _c:SDWORD
	mov eax, DWORD PTR _a
	push eax
	mov eax, DWORD PTR _b
	push eax
	mov eax, 3
	mov ebx, eax
	pop eax
	cdq
	idiv ebx
	mov ebx, eax
	pop eax
	add eax, ebx
	mov DWORD PTR _c, eax
	mov eax, DWORD PTR _c
	mov eax, DWORD PTR _c
	ret
_func2 endp

_func3 PROC C
	LOCAL _a:SDWORD, _b:SDWORD
	mov eax, DWORD PTR _a
	push eax
	mov eax, DWORD PTR _b
	mov ebx, eax
	pop eax
	add eax, ebx
	ret
_func3 endp
end

