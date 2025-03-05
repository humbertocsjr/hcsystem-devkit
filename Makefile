WATCOM = /usr/bin/watcom/
LDSRC = include/devkit.h $(wildcard ld/*.c)
LD86SRC = $(LDSRC) ld/target/i86.c
LD86 = i86-ld
ASSRC = include/devkit.h $(wildcard as/*.c)
AS86SRC = $(ASSRC) as/target/i86.c
AS86 = i86-as
ARSRC = include/devkit.h $(wildcard ar/*.c)
AR86SRC = $(ARSRC)
AR86 = i86-ar
SIZE86SRC = include/devkit.h size/size.c
SIZE86 = i86-size
BIN = binlin/i86-ld bindbg/i86-ld bindos/i86-ld.exe binwnt/i86-ld.exe binlin/i86-as bindbg/i86-as bindos/i86-as.exe binwnt/i86-as.exe binlin/i86-size bindbg/i86-size bindos/i86-size.exe binwnt/i86-size.exe binlin/i86-ar bindbg/i86-ar bindos/i86-ar.exe binwnt/i86-ar.exe

all: bindos binwnt binlin bindbg $(BIN)

clean:
	@rm -f $(BIN) *.o *.err  tests/*.com  tests/*.sys tests/*.small tests/*.tiny tests/*.hcix tests/*.o tests/*.dis *.zip

bindos:
	@mkdir -p bindos

binwnt:
	@mkdir -p binwnt

binlin:
	@mkdir -p binlin

bindbg:
	@mkdir -p bindbg

binlin/i86-ld: $(LD86SRC)
	@WATCOM=$(WATCOM) wcl386 -d2 -zq -aa -za99 -l=386 -bt=linux  -bcl=linux -dHOST=HOST_POSIX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-ld: $(LD86SRC)
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

bindos/i86-ld.exe: $(LD86SRC)
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-ld.exe: $(LD86SRC)
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/i86-as: $(AS86SRC)
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_POSIX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-as: $(AS86SRC)
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

bindos/i86-as.exe: $(AS86SRC)
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-as.exe: $(AS86SRC)
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/i86-size: $(SIZE86SRC)
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_POSIX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-size: $(SIZE86SRC)
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

bindos/i86-size.exe: $(SIZE86SRC)
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-size.exe: $(SIZE86SRC)
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/i86-ar: $(AR86SRC)
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_POSIX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-ar: $(AR86SRC)
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

bindos/i86-ar.exe: $(AR86SRC)
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-ar.exe: $(AR86SRC)
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o codegen/*.o

test-linux: all
	binlin/i86-as -o tests/test0.o tests/test0.s
	binlin/i86-ld -f com -b -o tests/test0.com tests/test0.o
	binlin/i86-ld -f sys -b -o tests/test0.sys tests/test0.o
	binlin/i86-ld -f v7 -o tests/test0.small tests/test0.o
	binlin/i86-ld -f v7tiny -b -o tests/test0.tiny tests/test0.o
	binlin/i86-ld -f hcix -b -o tests/test0.hcix tests/test0.o
	binlin/i86-size tests/test0.hcix
	ndisasm -b 16 -o 0x100 tests/test0.com > tests/test0.com.dis
	ndisasm -b 16 -e 0x10 -o 0 tests/test0.hcix > tests/test0.hcix.dis
	ndisasm -b 16 -e 0x10 -o 0 tests/test0.o > tests/test0.o.dis

test-dos: all
	emu2 bindos/i86-as.exe -o tests/test0.o tests/test0.s
	emu2 bindos/i86-ld.exe -f com -o tests/test0.com tests/test0.o
	emu2 bindos/i86-ld.exe -f sys -o tests/test0.sys tests/test0.o
	emu2 bindos/i86-ld.exe -f v7 -o tests/test0.small tests/test0.o
	emu2 bindos/i86-ld.exe -f v7tiny -b -o tests/test0.tiny tests/test0.o
	emu2 bindos/i86-ld.exe -f hcix -b -o tests/test0.hcix tests/test0.o
	emu2 bindos/i86-size.exe tests/test0.hcix
	ndisasm -b 16 -o 0x100 tests/test0.com > tests/test0.com.dis
	ndisasm -b 16 -e 0x10 -o 0 tests/test0.hcix > tests/test0.hcix.dis

distro: all
	@rm -f hcdk-*.zip
	@cd bindos; zip -q9 ../hcdk-dos.zip *
	@cd binwnt; zip -q9 ../hcdk-win.zip *
	@cd binlin; zip -q9 ../hcdk-lin.zip *
