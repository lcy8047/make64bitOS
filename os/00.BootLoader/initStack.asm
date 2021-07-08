mov ax, 0x0000
mov ss, ax
mov sp, 0xFFFE ; 0xFFFF가 아니라 0xFFFE 인게 16bit라 2byte값의 공간을 남겨준건가?
mov bp, 0xFFFE