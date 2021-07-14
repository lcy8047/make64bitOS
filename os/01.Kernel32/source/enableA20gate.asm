; enable A20
; method 1
in al, 0x92
or al, 0x02
and al, 0xFE
out 0x92, al

; method 2
mov ax, 0x2401
int 0x15

jc .A20GATEERROR
jmp .A20GATESUCCESS

.A20GATEERROR
; error
.A20GATESUCCESS
; success