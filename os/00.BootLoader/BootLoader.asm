[ORG 0x00]	; starting address 0x00
[BITS 16]	; below 

SECTION .text

jmp	0x07C0:START	; copy 0x7c0 to cs and jmp to START

;;;;;;;;;;;;;;
; env values
;;;;;;;;;;;;;;
TOTALSECTORCOUNT:	dw	1025	;


;;;;;;;;;;;;;;;;
; code section
;;;;;;;;;;;;;;;;
START:
	; set start addresses
	mov	ax,	0x07C0	; bootloader start address
	mov	ds,	ax
	mov	ax,	0xB800	; video memory start address
	mov	es,	ax

	; init stack
	mov ax, 0x0000
	mov ss, ax
	mov sp, 0xFFFE
	mov bp, 0xFFFE

;;;;;;;;;;;;;;;;;
; screenclear
;;;;;;;;;;;;;;;;;
	mov	si,	0
.SCREENCLEARLOOP:
	mov	byte[es:si], 0		; delete character
	mov	byte[es:si+1], 0x7C	; set attr  (high:bg - white, low:char - light red)
	add	si,	2
	cmp	si,	80*25*2
	jl .SCREENCLEARLOOP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; print start message
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push MESSAGE1
	push 0
	push 31
	call PRINTMESSAGE
	add sp, 6

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; print os load message
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push IMAGELOADINGMESSAGE
	push 1
	push 0
	call PRINTMESSAGE
	add sp, 6

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Load OS image from disk
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; before reset
RESETDISK:
	; call bios reset function ( like open disk )
	mov ax, 0   ; function num - reset 0
	mov dl, 0   ; drive num ( floppy 0x00, hdd 0x80, 0x81...)
	int 0x13
	jc HANDLEDISKERROR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; read sector from disk
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov si, 0x1000
	mov es, si
	mov bx, 0x0000
	mov di, word[ TOTALSECTORCOUNT ]

READDATA:
    cmp di, 0
    je READEND
    sub di, 0x1

	; call bios read function
    mov ah, 0x02
    mov al, 0x1
    mov ch, byte[ TRACKNUMBER ]
    mov cl, byte[ SECTORNUMBER ]
    mov dh, byte[ HEADNUMBER ]
    mov dl, 0x00
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

    cmp byte [ HEADNUMBER ], 0x0
    jne READDATA

    add byte[ TRACKNUMBER ], 0x1
    jmp READDATA
READEND:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; print os load done message
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push LOADINGCOMPLETEMESSAGE
	push 1
	push 20
	call PRINTMESSAGE
	add sp, 6

;;;;;;;;;;;;;;;;;;;;;;;;;
; launch loaded OS image
;;;;;;;;;;;;;;;;;;;;;;;;;
	jmp 0x1000:0x0000


;;;;;;;;;;;;;;;;;;;;
; functions
;;;;;;;;;;;;;;;;;;;;

; handle disk error function
HANDLEDISKERROR:
	push DISKERRORMESSAGE
	push 1
	push 20
	call PRINTMESSAGE
	add sp, 6

	jmp $

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

.MESSAGEEND:
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret

;;;;;;;;;;;;;;;;;;;
; data
;;;;;;;;;;;;;;;;;;;
MESSAGE1:				db 'WelCome DeoDeok OS', 0	; 18
DISKERRORMESSAGE:		db 'DISK ERROR', 0
IMAGELOADINGMESSAGE:	db 'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE:	db 'Loading Complete', 0
DEBUGMSG:				db 'DEBUG', 0

SECTORNUMBER:	db 0x02
HEADNUMBER:		db 0x00
TRACKNUMBER:	db 0x00

times 510 - ( $ - $$ )	db	0x00
db	0x55
db	0xAA
