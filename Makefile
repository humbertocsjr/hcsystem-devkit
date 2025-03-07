WATCOM = /usr/bin/watcom/
LDSRC = include/devkit.h $(wildcard ld/*.h) $(wildcard ld/*.c)
LD86SRC = $(LDSRC) ld/target/i86.c
LD86 = i86-ld
ASSRC = include/devkit.h $(wildcard as/*.h) $(wildcard as/*.c)
AS86SRC = $(ASSRC) as/target/i86.c
AS86 = i86-as
ARSRC = include/devkit.h $(wildcard ar/*.h) $(wildcard ar/*.c)
AR86SRC = $(ARSRC)
AR86 = i86-ar
NMSRC = include/devkit.h $(wildcard nm/*.h) $(wildcard nm/*.c)
NM86SRC = $(NMSRC)
NM86 = i86-nm
BASCSRC = include/devkit.h $(wildcard basc/*.h) $(wildcard basc/*.c)
BASC86SRC = $(BASCSRC) basc/target/i86.c
BASC86 = i86-basc
SIZE86SRC = include/devkit.h size/size.c
SIZE86 = i86-size
BIN = binlin/i86-ld binpsx/i86-ld bindbg/i86-ld bindos/i86-ld.exe binwnt/i86-ld.exe binlin/i86-as binpsx/i86-as bindbg/i86-as bindos/i86-as.exe binwnt/i86-as.exe binlin/i86-size binpsx/i86-size bindbg/i86-size bindos/i86-size.exe binwnt/i86-size.exe binlin/i86-ar binpsx/i86-ar bindbg/i86-ar bindos/i86-ar.exe binwnt/i86-ar.exe binlin/i86-basc binpsx/i86-basc bindbg/i86-basc bindos/i86-basc.exe binwnt/i86-basc.exe binlin/i86-nm binpsx/i86-nm bindbg/i86-nm bindos/i86-nm.exe binwnt/i86-nm.exe binlin/license bindos/license.txt binwnt/license.txt

all: bindos binwnt binlin bindbg binpsx $(BIN)
	@make --no-print-directory -C lib/libbasic all
	@make --no-print-directory -C examples all

clean:
	@rm -f $(BIN) *.o *.err  tests/*.com  tests/*.sys tests/*.small tests/*.tiny tests/*.hcix tests/*.o tests/*.dis *.zip
	@rm -Rf bindos bindbg binlin binpsx binwnt
	@make --no-print-directory -C lib/libbasic clean

bindos:
	@mkdir -p bindos

binwnt:
	@mkdir -p binwnt

binlin:
	@mkdir -p binlin

bindbg:
	@mkdir -p bindbg

binpsx:
	@mkdir -p binpsx

binlin/i86-ld: $(LD86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -d2 -zq -aa -za99 -l=386 -bt=linux  -bcl=linux -dHOST=HOST_LINUX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-ld: $(LD86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

binpsx/i86-ld: $(LD86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX  -o $@ $^

bindos/i86-ld.exe: $(LD86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-ld.exe: $(LD86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/i86-as: $(AS86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_LINUX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-as: $(AS86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

binpsx/i86-as: $(AS86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX -o $@ $^

bindos/i86-as.exe: $(AS86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-as.exe: $(AS86SRC)
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/i86-size: $(SIZE86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_LINUX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-size: $(SIZE86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

binpsx/i86-size: $(SIZE86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX -o $@ $^

bindos/i86-size.exe: $(SIZE86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-size.exe: $(SIZE86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/i86-ar: $(AR86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_LINUX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-ar: $(AR86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

binpsx/i86-ar: $(AR86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX -o $@ $^

bindos/i86-ar.exe: $(AR86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-ar.exe: $(AR86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/i86-basc: $(BASC86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_LINUX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-basc: $(BASC86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

binpsx/i86-basc: $(BASC86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX -o $@ $^

bindos/i86-basc.exe: $(BASC86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-basc.exe: $(BASC86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/i86-nm: $(NM86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=linux -dHOST=HOST_LINUX -cc -i=$(WATCOM)/lh -fe=$@ $^
	@rm -f *.o codegen/*.o

bindbg/i86-nm: $(NM86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX  -g -o $@ $^

binpsx/i86-nm: $(NM86SRC)
	@echo [CC] $@
	@cc -DHOST=HOST_POSIX -o $@ $^

bindos/i86-nm.exe: $(NM86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl -zq -bcl=dos -dHOST=HOST_DOS -bc -lr -cc -0 -fpi -ms -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o target/*.o

binwnt/i86-nm.exe: $(NM86SRC)
	@echo [CC] $@
	@WATCOM=$(WATCOM) wcl386 -zq -bcl=nt -dHOST=HOST_WINDOWS -bc -cc -3 -i=$(WATCOM)/h  -fe=$@ $^
	@rm -f *.o codegen/*.o

binlin/license: license
	@echo [CP] $@
	@cp $< $@

bindos/license.txt: license
	@echo [CP] $@
	@cp $< $@

binwnt/license.txt: license
	@echo [CP] $@
	@cp $< $@

test-bas-linux: all
	binlin/i86-basc -o tests/test1.o tests/test1.bas
	binlin/i86-size tests/test1.o
	binlin/i86-nm tests/test1.o
	ndisasm -b 16 -e 0x10 -o 0 tests/test1.o > tests/test1.o.dis

test-bas-dos: all
	emu2 bindos/i86-basc.exe -o tests/test1.o tests/test1.bas
	emu2 bindos/i86-size.exe tests/test1.o
	emu2 bindos/i86-nm.exe tests/test1.o
	ndisasm -b 16 -e 0x10 -o 0 tests/test1.o > tests/test1.o.dis

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
	@echo [ZIP] hcdk-dos.zip
	@cd bindos; zip -q9 ../hcdk-dos.zip * 
	@zip -q9 hcdk-dos.zip examples/*.bas examples/*.s examples/*.com lib/*
	@echo [ZIP] hcdk-win.zip
	@cd binwnt; zip -q9 ../hcdk-win.zip * 
	@zip -q9 hcdk-win.zip examples/*.bas examples/*.s examples/*.com lib/*
	@echo [ZIP] hcdk-lin.zip
	@cd binlin; zip -q9 ../hcdk-lin.zip * 
	@zip -q9 hcdk-lin.zip examples/*.bas examples/*.s examples/*.com lib/*

install: all
	@echo [INSTALL] /usr/local/bin
	@install -d /usr/local/bin
	@install binpsx/i86-ar /usr/local/bin/i86-ar
	@install binpsx/i86-as /usr/local/bin/i86-as
	@install binpsx/i86-ld /usr/local/bin/i86-ld
	@install binpsx/i86-nm /usr/local/bin/i86-nm
	@install binpsx/i86-size /usr/local/bin/i86-size
	@install binpsx/i86-basc /usr/local/bin/i86-basc
	@echo [INSTALL] /usr/local/lib/hcsystem
	@install -d /usr/local/lib/hcsystem
	@install lib/libdos.a /usr/local/lib/hcsystem/libdos.a
	@install lib/libhcsys.a /usr/local/lib/hcsystem/libhcsys.a

token:
	@nasm -f bin -o tests/token.bin tests/token.s
	@ndisasm -b 16 tests/token.bin > tests/token.dis