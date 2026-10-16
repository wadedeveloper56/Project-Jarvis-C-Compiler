OPTION SEGMENT:USE16
.model small;
.386
option casemap : none



.data
_x SDWORD 5
.data?
_u SDWORD  ?
_m SDWORD  ?

.code
_func1 PROTO C _a:SWORD , _b:SWORD ;
_func2 PROTO C _a:SWORD , _b:SWORD ;
_func3 PROTO C;

_func1 PROC C, _a:SWORD , _b:SWORD 
	LOCAL _c:SWORD
	mov ax, WORD PTR _a
	push ax
	mov ax, WORD PTR _b
	push ax
	mov ax, 5
	mov bx, ax
	pop ax
	imul ax, bx
	mov bx, ax
	pop ax
	add ax, bx
	mov WORD PTR _c, ax
	mov ax, WORD PTR _c
	mov ax, WORD PTR _c
	ret
_func1 endp

_func2 PROC C, _a:SWORD , _b:SWORD 
	LOCAL _c:SWORD
	mov ax, WORD PTR _a
	push ax
	mov ax, WORD PTR _b
	push ax
	mov ax, 3
	mov bx, ax
	pop ax
	cwd
	idiv bx
	mov bx, ax
	pop ax
	add ax, bx
	mov WORD PTR _c, ax
	mov ax, WORD PTR _c
	mov ax, WORD PTR _c
	ret
_func2 endp

_func3 PROC C
	LOCAL _a:SWORD, _b:SWORD
	mov ax, WORD PTR _a
	push ax
	mov ax, WORD PTR _b
	mov bx, ax
	pop ax
	add ax, bx
	ret
_func3 endp
end

