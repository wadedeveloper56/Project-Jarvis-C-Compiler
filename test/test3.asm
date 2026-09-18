option casemap:none
includelib kernel32.lib

;extern ExitProcess:proc

.code

; A 64-bit procedure taking two QWORD (64-bit) parameters 
; and allocating local stack space.
AddValues PROC arg1:QWORD, arg2:QWORD
    LOCAL localVal:QWORD

    ; Parameters passed in RCX, RDX (first two Win64 integer arguments)
    ; JWasm maps arg1 to rcx/shadow space and arg2 to rdx/shadow space
    mov rax, arg1
    mov rbx, arg2
    add rax, rbx
    
    ; Using a local variable
    mov localVal, rax
    mov rax, localVal

    ret
AddValues ENDP

main PROC
    sub rsp, 28h        ; Allocate shadow space (32 bytes) + alignment
    
    mov rcx, 15         ; First argument
    mov rdx, 25         ; Second argument
    call AddValues
    
    mov rcx, rax        ; Result from RAX becomes exit code
    ;call ExitProcess
main ENDP

END
