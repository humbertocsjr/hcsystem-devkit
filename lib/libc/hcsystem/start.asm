section .text
global _start
_start:
    mov dx, cs
    add dx, __segoff
    mov ds, dx
    mov es, dx
    cli
    mov ss, dx
    mov sp, stack_top
    sti
    push ax
    push bx
    call _main
    int 0x81
section .data
    resw 4096
stack_top: