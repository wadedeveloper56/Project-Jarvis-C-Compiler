.x64p
option casemap : none

.data?
m SDWORD ? ; global var m type = int

.data
x SDWORD 3; global var x type = int value = 3

.code
;-----------------
; params:
;   a : int
;   b : int
; locals:
;   c : int
;-----------------
_add PROC,a:SDWORD,b:SDWORD
      LOCAL _c:SDWORD
      ret
_add ENDP

;-----------------
; locals:
;   y : int
;-----------------
_main PROC
      LOCAL _y:SDWORD
      ret
_main ENDP

end
