;
;	NMH's Simple C Compiler, 2013,2014
;	C runtime module for DOS
;

; Calling conventions: stack, return in AX
; System call: AH=call#, arguments in call-specific registers,
;              carry indicates error,
;              return/error value in AX

DOS	equ	0x21
CMDLEN	equ	128
PMTRS	equ	64
PSPCL	equ	81
ENVPTR	equ	0x2c
STKTOP	equ	0xfffe
STKLEN	equ	0x1000

	
section .data 

global _environ 
global __faddcr

_environ:	dw	0
__faddcr:	dw	1

argc:		dw	0	; internal
argv:		dw	0

section .text

	extern	_main
    extern	_exit

; Initial entry point is here

global	_start
_start:
	mov	bx,8192		; set program memory to 128K ; XXX
	mov	ah,0x4a		; setblock
	int	DOS
	jnc	setupheap

error:
	mov	dx, memerror
	push	cs
	pop	ds
	mov	ah,9		; write $-terminated
	int	DOS
	mov	ax,0x4c7f	; exit(127)
	int	DOS

; At this point:
; CS = code, DS = ES = PSP, SS = end of static data
; DGROUP = beginning of static data

setupheap:
	mov	dx,ss		; dx = length of static data
    mov ax,cs
	add	ax,__segoff
	sub	dx,ax
	mov	cl,4
	shl	dx,cl
    cs
	mov	[minmem],dx	; beginning of malloc() arena (heap)
    cs
	mov	[break],dx	; current break
	xor	ax,ax
	sub	ax,dx
	sub	ax,STKLEN
    cs
	add	ax,[break]
    cs
	mov	[maxmem],ax	; size of arena
    mov ax,cs
	add	ax,__segoff
	; set DS = SS = DGROUP
	mov	ds,ax
	mov	ss,ax
	mov	sp,STKTOP

; At this point:
; CS = code, DS = SS = DGROUP, ES = PSP

; copy the command line from the PSP

	sub	sp,CMDLEN	; reserve space for argc,argv
	sub	sp,PMTRS
	mov	di,sp
	add	di,2		; point to reserved area
	mov	si,PSPCL	; command line in PSP
	mov	bx,di		; bx = start of command line
	mov	cx,CMDLEN	; copy CMDLEN bytes

	call	swapsegs
	cld
	rep
	movsb
	call	swapsegs

; parse the command line and build argv

	mov	si,di		; save &argv[0]
	mov	word [si],0	; clear argv[0]
	inc	si
	inc	si
	mov	cx,1		; preset argc

nextch:	mov	al,[bx]		; get char from cmd line
	inc	bx
	cmp	al,0x0d		; end of line ?
	jz	argsdone

	cmp	al,0x20		; skip white spaces
	jz	nextch
	cmp	al,9
	jz	nextch

	dec	bx
	mov	[si],bx		; save argv[n]
	inc	bx
	inc	si
	inc	si
	inc	cx		; increment argc

getend:	mov	al,[bx]		; search end of current arg
	cmp	al,0x20
	jz	endarg
	cmp	al,9
	jz	endarg
	cmp	al,0x0d		; CR: end of line reached, stop parsing
	jz	argsdone
	inc	bx
	jmp	short getend

endarg:	
    mov	byte [bx],0
	inc	bx
	jmp	nextch

argsdone:
	mov	byte [bx],0	; terminate last arg
	mov	word [si],0	; terminate argv[] with NULL
	mov	[argc],cx		; save for later use
	mov	[argv],di

; copy environment

setupenv:
	mov	si,ENVPTR	; get environ ptr from PSP
    es
	mov	ax,[si]
	mov	es,ax

	xor	bx,bx		; get length of env
	xor	dx,dx		; and number of entries
nxtenv:	
    es
    mov	al,[bx]
	inc	bx
	or	al,al
	jnz	nxtenv
	inc	dx
    es
	mov	al,[bx]
	inc	bx
	or	al,al
	jnz	nxtenv

    cs
	cmp	bx,[maxmem]
	jbe	copyenv
	jmp	error		; not enough space

copyenv:
	sub	sp,bx		; space for environ[]
	sub	sp,dx
	sub	sp,dx
	sub	sp,2

	mov	di,sp		; copy environment
	xor	si,si
	mov	cx,bx

	call	swapsegs
	rep
	movsb
	push	es
	pop	ds

	mov	[_environ],di

	mov	si,sp		; build environ[]
	inc	si
store:	dec	si
	mov	ax,si
	stosw
scan:	lodsb
	or	al,al
	jnz	scan
	lodsb
	or	al,al
	jnz	store
	xor	ax,ax
	stosw

; initialize run time library

	call __init	;INIT

; call main() and exit

	mov	ax,[argv]
	push	ax
	mov	ax,[argc]
	push	ax
	call _main
	add	sp,4
	push	ax
x:	call _exit	;EXIT
	xor	bx,bx
	div	bx
	jmp	x

; swap es,dx

swapsegs:
	mov	dx,es		; swap es,ds
	mov	ax,ds
	mov	ds,dx
	mov	es,ax
	ret

; evaluate 'switch' statement

global switch
switch:	
    cs
    mov	cx,[si]	; count
	inc	si
	inc	si	
nxcase: 
    cs
    mov	dx,[si]	; fetch value from table
	inc	si
	inc	si	
    cs
	mov	bx,[si]	; fetch address from table
	inc	si
	inc	si	
	cmp	ax,dx		; right case?
	jz	docase		; no, go on
	dec	cx
	jnz	nxcase
    cs
	mov	bx,[si]
docase:	jmp	bx

; int setjmp(jmp_buf env);

global _setjmp
_setjmp:
	mov	bx,sp
	mov	bx,[bx+2]	; env
	mov	ax,sp
	add	ax,2
	mov	[bx],ax
	mov	[bx+2],bp
	mov	si,sp
	mov	ax,[si]
	mov	[bx+4],ax
	xor	ax,ax
	ret

; void longjmp(jmp_buf env, int v);

global _longjmp
_longjmp:
	mov	bx,sp
	mov	ax,[bx+4]	; v
	or	ax,ax
	jnz	vok
	inc	ax
vok:	mov	bx,[bx+2]	; env
	mov	sp,[bx]
	mov	bp,[bx+2]
	mov	si,[bx+4]
	jmp	si

; void *_sbrk(int n);

global __sbrk
__sbrk:	mov	bx,sp
	mov	ax,[bx+2]	; size
    cs
	add	ax,[break]
    cs
	cmp	ax,[maxmem]
	ja	sbfail
    cs
	cmp	ax,[minmem]
	jb	sbfail
    cs
	mov	ax,[break]
	mov	dx,ax
	add	dx,[bx+2]	; size
    cs
	mov	[break],dx
	ret
sbfail:	xor	ax,ax
	dec	ax
	ret

; void _exit(int rc);

global __exit
__exit:	mov	bx,sp
	mov	ax,[bx+2]	; rc
	mov	ah,0x4c		; terminate program
	int	DOS

; int _creat(char *name, int perms);

global __creat
__creat:mov	bx,sp
	mov	cx,0
	mov	dx,[bx+2]	; name
	mov	ah,0x3c
	int	DOS
	jnc	crok
	xor	ax,ax
	dec	ax
crok:	ret

; int _open(char *name, int mode);

global __open
__open:	mov	bx,sp
	mov	ax,[bx+4]	; mode
	mov	ah,0x3d
	mov	dx,[bx+2]	; name
	int	DOS
	jnc	opok
	xor	ax,ax
	dec	ax
opok:	ret

; int _close(int fd);

global __close
__close:
	mov	bx,sp
	mov	bx,[bx+2]	; fd
	mov	ah,0x3e
	int	DOS
	jnc	clok
	xor	ax,ax
	dec	ax
	ret
clok:	xor	ax,ax
	ret

; int _read(int fd, void *buf, int len);

global __read
__read:
	mov	bx,sp
	mov	cx,[bx+6]	; len
	mov	dx,[bx+4]	; buf
	mov	bx,[bx+2]	; fd
	mov	ah,0x3f
	int	DOS
	jnc	rdok
	xor	ax,ax
	dec	ax
rdok:	ret

; int _write(int fd, void *buf, int len);

global __write
__write:
	mov	bx,sp
	mov	cx,[bx+6]	; len
	mov	dx,[bx+4]	; buf
	mov	bx,[bx+2]	; fd
	mov	ah,0x40
	int	DOS
	jnc	wrok
	xor	ax,ax
	dec	ax
wrok:	ret

section .data 

abort:	db	"crt0: aborted.", 13, 10, '$'

section .text

; int raise(int sig);

global _raise
_raise:	
    mov	dx, abort
	mov	ah,9
	int	DOS
	mov	ax,0x4c7f
	int	DOS
	ret

; int signal(int sig, int (*handler)())

global _signal
_signal:
	mov	ax,-2		; dummy, return SIG_ERR
	ret

; int _lseek(int fd, int where, int how);

global __lseek
__lseek:
	mov	bx,sp
	mov	ax,[bx+4]	; where
	cwd
	mov	cx,dx
	mov	dx,ax
	mov	ax,[bx+6]	; how
	mov	bx,[bx+2]	; fd
	mov	ah,0x42
	int	DOS
	jnc	lsok
	xor	ax,ax
	dec	ax
lsok:	ret

; int _unlink(char *name);

global __unlink
__unlink:
	mov	bx,sp
	mov	dx,[bx+2]	; name
	mov	ah,0x41
	int	DOS
	jnc	ulok
	xor	ax,ax
	dec	ax
	ret
ulok:	xor	ax,ax
	ret

; int _rename(char *old, char *new);

global __rename
__rename:
	mov	bx,sp
	mov	dx,[bx+2]	; old
	mov	di,[bx+4]	; new
	mov	ah,0x56
	int	DOS
	jnc	rnok
	xor	ax,ax
	dec	ax
	ret
rnok:	xor	ax,ax
	ret

; int _fork(void);

global __fork
__fork:	xor	ax,ax		; dummy, return -1
	dec	ax
	ret

; int wait(int *rc)

global __wait
__wait:	xor	ax,ax		; dummy, return -1
	dec	ax
	ret

; int _execve(char *prog, char **argv, char **env);

global __execve
__execve:
	xor	ax,ax		; dummy, return -1
	dec	ax
	ret

; int _system(char *shell, void *parmb);

global __system
__system:
	mov	bx,sp
	mov	dx,[bx+2]	; shell
	mov	bx,[bx+4]	; parmb
	mov	di,ds
	mov	[bx+4],di
	mov	[bx+8],di
	mov	[bx+12],di
	mov	ax,0x4b00
	int	DOS
	jnc	syok
	xor	ax,ax
	dec	ax
	ret
syok:	xor	ax,ax
	ret

; int _time(void);

global __time
__time:
	xor	ax,ax		; dummy, return 0
	ret

; internal data (in code segment)

break:		dw	0
minmem:		dw	0
maxmem:		dw	0

memerror:	db	"crt0: out of memory.", 13, 10, '$'


