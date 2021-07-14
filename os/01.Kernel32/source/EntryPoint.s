[ORG 0x00]
[BITS 16]

SECTION .text

;;;;;;;;;;
; code
;;;;;;;;;;
START:
    mov ax, 0x1000
    mov ds, ax
    mov es, ax

    cli
    lgdt[ GDTR ]

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; entry protected mode
; Disable Paging, Disable Cache, Internal FPU
; Disable Align Check, Enable ProtectedMode
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, 0x4000003B ; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
    mov cr0, eax
    jmp dword 0x08: ( PROTECTEDMODE - $$ + 0x10000 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; entry protected mode
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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

    ; print success entry protected mode
    push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000)
    push 2
    push 0
    call PRINTMESSAGE
    add esp, 12
    
    jmp dword 0x08: 0x10200 ; jmp to C language kernel

;;;;;;;;;;;;;;;;;;;;;;;;;
; functions
;;;;;;;;;;;;;;;;;;;;;;;;;
; print message function
; PARAM: x, y, message

PRINTMESSAGE:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push eax
    push ecx
    push edx
    
    ; y
    mov eax, dword[ ebp + 12 ]
    mov esi, 160     ; one char per 2 bytes and 80 column -> 160 bytes
    mul esi          ; multiply ax register oprand value
    mov edi, eax

    ; cal x and find x,y
    mov eax, dword[ ebp + 8 ]
    mov esi, 2   ; one char per 2 bytes
    mul esi
    add edi, eax

    ; message address
    mov esi, dword[ ebp + 16 ]

.MESSAGELOOP:
    mov cl, byte[ esi ]  ; copy 1 byte character
    cmp cl, 0   ; cmp null
    je .MESSAGEEND  ; if null, exit

    mov byte[ edi+0xB8000 ], cl

    add esi, 1   ; next message
    add edi, 2   ; next video address(1 byte : char, 1byte : meta so skip 2 bytes)

    jmp .MESSAGELOOP

.MESSAGEEND
    pop edx
    pop ecx
    pop eax
    pop edi
    pop esi
    pop ebp
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; data
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; for align below data 8 bytes
align 8, db 0

; for end of GDTR align 8 bytes
dw 0x0000
; GDTR data structure
GDTR:
    dw GDTEND - GDT - 1
    dd ( GDT - $$ + 0x10000)

; GDT(Global Descriptor Table)
GDT:
    ; Null descriptor. must initialize 0
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

    CODEDESCRIPTOR:
        dw 0xFFFF   ; Limit [15:0]
        dw 0x0000   ; Base [15:0]
        db 0x00     ; Base [23:16]
        db 0x9A     ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xCF     ; G=1, D=1, L=0, Limit[19:16]
        db 0x00     ; Base [31:24]

    DATADESCRIPTOR:
        dw 0xFFFF   ; Limit [15:0]
        dw 0x0000   ; Base [15:0]
        db 0x00     ; Base [23:16]
        db 0x92     ; P=1, DPL=0, Data Segment, Read/Write
        db 0xCF     ; G=1, D=1, L=0, Limit[19:16]
        db 0x00     ; Base [31:24]
GDTEND:

SWITCHSUCCESSMESSAGE:   db 'Switch To Protected Mode Success~!!', 0
times 512 - ( $ - $$ ) db 0x00