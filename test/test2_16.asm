.286
.model small;
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
	ret
_func1 endp

_func2 PROC C, _a:SWORD , _b:SWORD 
	LOCAL _c:SWORD
	ret
_func2 endp

_func3 PROC C
	LOCAL _a:SWORD, _b:SWORD
	ret
_func3 endp
end

