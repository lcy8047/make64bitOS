;;;;;;;;;;;;;;;;;;;;
;
;
;;;;;;;;;;;;;;;;;;;;

TOTALSECTORCOUNT:	dw	1024
SECTORNUMBER:		db	0x02
HEADNUMBER:			db	0x00
TRACKNUMBER:		db	0x00

mov si, 0x10000
mov es, si
mov bx, 0x0000
mov di, word[TOTALSECTORCOUNT]

READDATA:
    cmp di, 0
    je READEND
    sub di, 0x1

    mov ah, 0x02
    mov al, 0x1
    mov ch, byte[ TRACKNUMBER ]
    mov cl, byte[ SECTORNUMBER ]
    mov dh, byte[ HEADNUMBER ]
    mov dl, 0x0
    int 0x13
    jc HANDLEDISKERROR

    add si, 0x0020
    mov es, si

    mov al, byte[ SECTORNUMBER ]
    add al, 0x1
    mov byte[ SECTORNUMBER ], al
    cmp al, 19
    jl READDATA

    xor byte[ HEADNUMBER ], 0x01
    mov byte[ SECTORNUMBER ], 0x01

    cmp byte[ HEADNUMBER ], 0x0
    jne READDATA

    add byte[ TRACKNUMBER ], 0x1
    jmp READDATA
READEND:

HANDLEDISKERROR:
    jmp $