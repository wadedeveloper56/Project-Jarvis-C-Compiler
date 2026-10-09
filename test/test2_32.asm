.386
.model flat, c;
option casemap : none



.data
_x SDWORD 3
.data?
_u SDWORD  ?
_m SDWORD  ?

.code
_func1 PROTO C _a:SDWORD , _b:SDWORD ;
_func2 PROTO C _a:SDWORD , _b:SDWORD ;
_func3 PROTO C;

_func1 PROC C, _a:SDWORD , _b:SDWORD 
	ret
_func1 endp

_func2 PROC C, _a:SDWORD , _b:SDWORD 
	ret
_func2 endp

_func3 PROC C
	ret
_func3 endp
end

