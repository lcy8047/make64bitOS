; read 1byte from port
; PARAM: portnumber
kInPortByte:
    push rdx        ; save reg value for use reg temporary
                    ; restore at end of function

    mov rdx, rdi    ; move param to rdx
    mov rax, 0      ; clear rax
    in al, dx       ; read from port [dx]
                    ; save to al
    pop rdx         ; restore rdx
    ret

; write 1byte to port
; PARAM: portnumber, data
kOutPortByte:
    push rdx        ; save reg value for use reg temporary
    push rax        ; restore at end of function

    mov rdx, rdi    ; move param1 to rdx (port num)
    mov rax, rsi    ; move param2 to rax (data)
    out dx, al      ; write to port [dx]

    pop rax         ; restore rax
    pop rdx         ; restore rdx
    ret