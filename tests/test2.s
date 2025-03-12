section .text
section .bss
_b:
resb 2
section .text
section .bss
_z:
resb 2
section .text
section .bss
_c:
resb 2
section .text
global _start
_start:
push bp
mov bp, sp
sub sp, 2
mov ax, 45
push ax
mov ax, 34
push ax
mov ax, 12
push ax
mov ax, [bp+-2] ; a
call ax
add sp, 6
mov sp, bp
pop bp
ret
global _main
_main:
push bp
mov bp, sp
sub sp, 4
mov ax, _b
mov [_c], ax
mov si, [_c]
mov ax, [si]
mov [_b], ax
mov ax, 456
push ax
mov ax, 123
push ax
call _start
add sp, 4
mov word [_b], 1
L6:
mov ax, [_b]
mov bx, 24
cmp ax, bx
mov ax, 1
jl L7
dec ax
L7:
or ax, ax
je L8
jmp L5
L8:
mov ax, [bp+-2] ; y
add ax, 23
mov [_z], ax
mov ax, [_b]
add ax, 1
mov [_b], ax
jmp L6
L5:
mov ax, _c
add ax, 100
mov si, ax
mov ax, [si]
mov [_b], ax
mov sp, bp
pop bp
ret
