[BITS 64]

SECTION .text

extern main

; code

START:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; setting stack in area 0x600000 ~ 0x6FFFFF
    mov ss, ax
    mov rsp, 0x6FFFF8
    mov rbp, 0x6FFFF8

    call main

    jmp $
