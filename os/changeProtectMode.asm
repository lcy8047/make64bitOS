jmp dword 0x08: ( PROTECTEDMODE - $$ + 0x10000)

[BITS 32]
PROTECTEDMODE:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; create stack
    mov ss, ax
    mov esp, 0xFFFE
    mov ebp, 0xFFFE