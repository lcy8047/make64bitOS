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