; comment
section .text
global _start
_start:
	mov ax, [_var]
	mov ax, [_var2]
extern test1_func
	call test1_func
section .data
_var:
	dw 0
_var2:
	dw 0
nop