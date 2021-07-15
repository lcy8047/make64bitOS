[BITS 32]

; export for call in C
global kReadCPUID, kSwitchAndExecute64bitKernel

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

; change IA-32e Mode and execute 64bit kernel
kSwitchAndExecute64bitKernel:
    ; set cr4 PAE bit
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax

    ; enable cr3 address and cache PML4 table
    mov eax, 0x100000
    mov cr3, eax

    ; set IA32_EFER.LME 1 for enable IA-32e
    mov ecx, 0xC0000080
    rdmsr

    or eax, 0x0100
    wrmsr

    ; set cr0 NW=0, CD=0, PG=1
    ; for enable cache, paging
    mov eax, cr0
    or eax, 0xE0000000
    xor eax, 0x60000000
    mov cr0, eax

    jmp 0x08:0x200000

    jmp $