include ../config.mk
OUT = ../binpsx/i86-ar ../bindbg/i86-ar ../binwnt/i86-ar.exe  ../bindos/i86-ar.exe  ../binlin/i86-ar
SRC = $(wildcard *.c)
HDR = $(wildcard *.h)

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT) *.o

../binpsx/%: $(SRC) $(HDR) Makefile
	@echo [CC] ${@F} [POSIX]
	@cc -o $@ $(SRC)

../bindbg/%: $(SRC) $(HDR) Makefile
	@echo [CC] ${@F} [DEBUG]
	@cc -g -o $@ $(SRC)

../binwnt/%: $(SRC) $(HDR) Makefile
	@echo [CC] ${@F} [WINDOWS]
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $(SRC)
	@rm -f *.o

../bindos/%: $(SRC) $(HDR) Makefile
	@echo [CC] ${@F} [DOS]
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $(SRC)
	@rm -f *.o

../binlin/%: $(SRC) $(HDR) Makefile
	@echo [CC] ${@F} [LINUX]
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_LINUX -cc -i=$(WATCOM)/lh -fe=$@ $(SRC)
	@rm -f *.o