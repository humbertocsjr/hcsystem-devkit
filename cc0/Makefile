include ../config.mk
OUT = ../binpsx/i86-cc0 ../bindbg/i86-cc0 ../binwnt/i86-cc0.exe  ../bindos/i86-cc0.exe  ../binlin/i86-cc0
SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
I86 = target/i86.c

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT) *.o

native: ../binpsx/i86-cc0
	@echo >> /dev/null

../binpsx/i86-%: $(SRC) $(HDR) $(I86) Makefile
	@echo [CC] ${@F} [POSIX]
	@cc -o $@ $(SRC) $(I86)

../bindbg/i86-%: $(SRC) $(HDR) $(I86) Makefile
	@echo [CC] ${@F} [DEBUG]
	@cc -g -o $@ $(SRC) $(I86)

../binwnt/i86-%: $(SRC) $(HDR) $(I86) Makefile
	@echo [CC] ${@F} [WINDOWS]
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $(SRC) $(I86)
	@rm -f *.o

../bindos/i86-%: $(SRC) $(HDR) $(I86) Makefile
	@echo [CC] ${@F} [DOS]
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $(SRC) $(I86)
	@rm -f *.o

../binlin/i86-%: $(SRC) $(HDR) $(I86) Makefile
	@echo [CC] ${@F} [LINUX]
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_LINUX -cc -i=$(WATCOM)/lh -fe=$@ $(SRC) $(I86)
	@rm -f *.o