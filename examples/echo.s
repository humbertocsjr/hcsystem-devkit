section .text
global _start
_start:
    mov cl, [0x80]
    xor ch, ch
    mov si, 0x81
    call print_string
    call print_enter
    int 0x20

; si = string
; cx = size
print_string:
    cmp cx, 0
    je .end
    .loop:
        lodsb
        mov dl, al
        mov ah, 0x2
        int 0x21
        loop .loop
    .end:
    ret

print_enter:
    mov ah, 0x2
    mov dl, 13
    int 0x21
    mov ah, 0x2
    mov dl, 10
    int 0x21
    ret

xor bx, 0x123
xor bl, 0x12
nop
