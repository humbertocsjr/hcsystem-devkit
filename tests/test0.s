; comment
section .text
vartmp equ 123
global _start
    call _start
    call _test
    mov ax, vartmp
    call [_test]
    call far 0:0
    call [bx]
    jmp _start
    jmp short _temp
    jmp _temp
    jmp [bx]
_temp:
    mov word [bp+si+4], _test
    mov byte [bp+4], 0x12
    mov word [bp+4], _test
    mov word bx, cx
    mov byte bl, al
    mov word ax, 0x123
    mov byte al, 0x12
    nop
    mov word [0x123], 0x123
    mov byte [0x123], 0x12
    nop
    mov bx, [0x123]
    mov cx, [bp+0x123]
    mov bl, [0x123]
    mov cl, [bp+0x123]
    mov [0x123], bx
    mov [bp+0x123], cx
    mov [0x123], bl
    mov [bp+0x123], cl
    mov ax, ds
    mov ds, ax
    mov ds, [0x123]
    mov [0x123], ds
    nop
    nop
    push ax
    push bx
    push cs
    push ds
    push word [0x123]
    push word [bp+0x123]
    pop ax
    pop bx
    pop es
    pop ds
    pop word [0x123]
    pop word [bp+0x123]
    in al, dx
    in ax, dx
    in al, 0x70
    in ax, 0x70
    out dx, al
    out dx, ax
    out 0x70, al
    out 0x70, ax
    xchg ax, bx
    xchg [0x123], ax
    xchg bx, [0x123]
    xlat
    lea ax, [0x123]
    lds ax, [0x123]
    les ax, [0x123]
    lahf
    sahf
    pushf
    popf
    xor ax, 0x123
    xor al, 0x12
    xor ax, bx
    xor bx, ax
    xor al, bl
    xor [0x123], ax
    xor ax, [0x123]
    xor [0x123], bl
    xor bl, [0x123]
    xor word [0x123], 0x456
    xor byte [0x123], 0x456
    neg ax
    neg dl
    neg dl
    neg word [0x123]
    neg byte [0x123]
    call _start
    call _test
    call [bx]
    call bx
    call far [_start]
    stosb
    stosw
    lodsb
    lodsw
    scasb
    scasw
    cmpsb
    cmpsw
    movsb
    movsw
    aas
    das
    aam
    aad
    cbw
    cwd
    rcr bx, 1
    rcr dl, 1
    rcr word [0x123], 1
    rcr byte [0x123], 1
    rcr bx, cl
    rcr dl, cl
    rcr word [0x123], cl
    rcr byte [0x123], cl
    nop
_start:
_test:
    mov ax, _test
    .oi:
    mov ax, _start
    nop
    dw .oi
    dw _test.oi
    times 400-($-$$) scasb
    nop