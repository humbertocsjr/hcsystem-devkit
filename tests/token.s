shl ax, cl
times 16-($-$$) nop
shr ax, cl
times 32-($-$$) nop
mov cx, bx
times 48-($-$$) nop
mov ax, dx
times 64-($-$$) nop