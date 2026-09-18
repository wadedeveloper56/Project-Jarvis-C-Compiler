.386
.model flat, c;
option casemap : none



.data
.data?
_var1 SBYTE  ?
_var2 SWORD  ?
_var3 SDWORD  ?

.code
_main PROTO C _argc:SDWORD , _argv:DWORD ;

_main PROC C, _argc:SDWORD , _argv:DWORD 
	mov eax,5
	ret
_main endp
end

