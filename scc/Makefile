include ../config.mk
OUT = ../binpsx/i86-scc ../bindbg/i86-scc ../binwnt/i86-scc.exe  ../bindos/i86-scc.exe  ../binlin/i86-scc
SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
I86 = 

all: $(OUT)
	@echo >> /dev/null

native: ../binpsx/i86-nm
	@echo >> /dev/null

clean:
	@rm -f $(OUT) *.o

../binpsx/i86-%: $(SRC) $(HDR) $(I86) Makefile
	@echo [CC] ${@F} [POSIX]
	@cc -D HOST=HOST_POSIX -o $@ $(SRC) $(I86)

../bindbg/i86-%: $(SRC) $(HDR) $(I86) Makefile
	@echo [CC] ${@F} [DEBUG]
	@cc -D HOST=HOST_POSIX -g -o $@ $(SRC) $(I86)

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