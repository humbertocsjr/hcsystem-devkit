section .text
section .data
_b:
dw 123
section .text
section .bss
_z:
resb 2
section .text
section .bss
_c:
resb 2
section .text
_main:
push bp
mov bp, sp
sub sp, 4
mov ax, _b
mov [_c], ax
mov si, [_c]
mov ax, [si]
mov [_b], ax
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
mov word [_z], 23
mov ax, [_b]
add ax, 1
mov [_b], ax
jmp L6
L5:
mov sp, bp
pop bp
ret
