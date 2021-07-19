; export for call in C
global kReadCPUID

SECTION .text

; return CPUID
; PARAM: DWORD dwEAX, DWORD *pdwEAX, *pdwEBX, *pdwECX, *pdwEDX
kReadCPUID:
    push ebp
    mov ebp, esp
    push eax
    push ebx
    push ecx
    push edx
    push esi

    ; run CPUID
    mov eax, dword[ ebp + 8 ]
    cpuid

    ; save return value to params
    ; *pdwEAX
    mov esi, dword[ ebp + 12 ]
    mov dword[ esi ], eax

    ; *pdwEBX
    mov esi, dword[ ebp + 16 ]
    mov dword[ esi ], ebx

    ; *pdwECX
    mov esi, dword[ ebp + 20 ]
    mov dword[ esi ], ecx

    ; *pdwEDX
    mov esi, dword[ ebp + 24 ]
    mov dword[ esi ], edx

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret