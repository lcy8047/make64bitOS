; print message function
; PARAM: x, y, message

PRINTMESSAGE:
    push bp
    mov bp, sp

    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800
    mov es, ax

    ;;;;;;;;;;
    ; caculate x,y point
    ;;;;;;;;;;
    
    ; y
    mov ax, word[ bp + 6 ]
    mov si, 160     ; one char per 2 bytes and 80 column -> 160 bytes
    mul si          ; multiply ax register oprand value
    mov di, ax

    ; cal x and find x,y
    mov ax, word[ bp + 4 ]
    mov si, 2   ; one char per 2 bytes
    mul si
    add di, ax

    ; message address
    mov si, word[ bp + 8 ]

.MESSAGELOOP:
    mov cl, byte[ si ]  ; copy 1 byte character
    cmp cl, 0   ; cmp null
    je .MESSAGEEND  ; if null, exit

    mov byte[ es: di ], cl

    add si, 1   ; next message
    add di, 2   ; next video address(1 byte : char, 1byte : meta so skip 2 bytes)

    jmp .MESSAGELOOP

.MESSAGEEND
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret