.x64p
option casemap : none



.data
.data?
_var1 SBYTE  ?
_var2 SWORD  ?
_var3 SDWORD  ?

.code
_main PROTO C _argc:SDWORD , _argv:QWORD ;

_main PROC C, _argc:SDWORD , _argv:QWORD 
	mov rax,5
	ret
_main endp
end

