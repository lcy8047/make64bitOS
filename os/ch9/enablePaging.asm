; Set PAE bit
mov eax, cr4
or eax, 0x20
mov cr4, eax

; enable address, cache of PML4 table
mov eax, 0x100000
mov cr3, eax

; enable paging for process
mov eax, cr0
or eax, 0x80000000  ; set PG bit
mov cr0, eax