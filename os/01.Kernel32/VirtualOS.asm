[ORG 0x0]
[BITS 16]

SECTION .text

jmp 0x1000:START

SECTORCOUNT:    dw 0x0000   ; current sector
TOTALSECTORCOUNT   equ 1024

START:
    mov ax, cs
    mov ds, ax
    mov ax, 0xB800
    mov es, ax

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; make code each sector
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    %assign i 0
    %rep TOTALSECTORCOUNT
        %assign i i+1

        mov ax, 2
        mul word[ SECTORCOUNT ]
        mov si, ax
        mov byte[ es: si+(160*2) ], '0' + ( i % 10 )
        add word[ SECTORCOUNT ], 1

        %if i == TOTALSECTORCOUNT
            jmp $
        %else
            jmp ( 0x1000 + i * 0x20): 0x0000
        %endif

        times 512 - ( $ - $$ ) % 512  db 0x00
    %endrep