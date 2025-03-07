section .text
global string_alloc
string_alloc:
    ret

section .bss
string_table:
    resw 5120/16
string_buffer:
    resb 5120