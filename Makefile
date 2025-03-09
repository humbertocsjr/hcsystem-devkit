MAKE = make --no-print-directory -C 

all native: bindos binwnt binlin bindbg binpsx 
	@$(MAKE) ar $@
	@$(MAKE) as $@
	@$(MAKE) cc0 $@
	@$(MAKE) nm $@
	@$(MAKE) size $@
	@$(MAKE) ld $@
	@$(MAKE) lib/libc $@
	@$(MAKE) examples $@

clean:
	@$(MAKE) ar $@
	@$(MAKE) as $@
	@$(MAKE) cc0 $@
	@$(MAKE) nm $@
	@$(MAKE) size $@
	@$(MAKE) ld $@
	@$(MAKE) lib/libc $@
	@$(MAKE) examples $@
	@rm -f $(BIN) *.zip *.img tests/*.dis  tests/*.o  tests/*.hcix  tests/*.com  tests/*.tiny  tests/*.small tests/*.bin tests/*.sys
	@rm -Rf bindos bindbg binlin binpsx binwnt

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


test-cc-linux: all
	rm -f tests/test2.s
	binlin/i86-cc0 -o tests/test2.s tests/test2.c
	binlin/i86-as -o tests/test2.o tests/test2.s
	binlin/i86-ld -f com -o tests/test2.com tests/test2.o
	binlin/i86-size tests/test2.o
	binlin/i86-nm tests/test2.o
	ndisasm -b 16 -o 0x100 tests/test2.com > tests/test2.dis

test-cc-dos: all
	rm -f tests/test2.s
	emu2 bindos/i86-cc0.exe -o tests/test2.o tests/test2.c
	emu2 bindos/i86-size.exe tests/test2.o
	emu2 bindos/i86-nm.exe tests/test2.o
	ndisasm -b 16 -e 0x10 -o 0 tests/test2.o > tests/test2.o.dis

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

distro-native: native
	@echo [ZIP] hcdk-native.zip
	@cd binpsx; zip -q9 ../hcdk-native.zip * 
	@zip -q9 hcdk-native.zip examples/*.bas examples/*.s examples/*.com lib/* license
	

distro: all
	@rm -f hcdk-*.zip
	@echo [ZIP] hcdk-dos.zip
	@cd bindos; zip -q9 ../hcdk-dos.zip * 
	@zip -q9 hcdk-dos.zip examples/*.bas examples/*.s examples/*.com lib/* license
	@echo [ZIP] hcdk-win.zip
	@cd binwnt; zip -q9 ../hcdk-win.zip * 
	@zip -q9 hcdk-win.zip examples/*.bas examples/*.s examples/*.com lib/* license
	@echo [ZIP] hcdk-lin.zip
	@cd binlin; zip -q9 ../hcdk-lin.zip * 
	@zip -q9 hcdk-lin.zip examples/*.bas examples/*.s examples/*.com lib/* license
	@dd if=/dev/zero of=hcsystem.img bs=1024 count=1440 status=none
	@echo [IMG] hcsystem.img
	@dd if=/dev/zero of=hcsystem.img bs=1024 count=1440 status=none
	@mformat -i hcsystem.img -f 1440
	@mmd -i hcsystem.img ::/dos
	@mmd -i hcsystem.img ::/win
	@mmd -i hcsystem.img ::/lib
	@mmd -i hcsystem.img ::/examples
	@mcopy -i hcsystem.img bindos/*.* ::/dos/
	@mcopy -i hcsystem.img binwnt/*.* ::/win/
	@mcopy -i hcsystem.img lib/*.* ::/lib/
	@mcopy -i hcsystem.img examples/*.* ::/examples/
	@mcopy -i hcsystem.img license ::/license.txt

install: all
	@echo [INSTALL] /usr/local/bin
	@install -d /usr/local/bin
	@install binpsx/i86-ar /usr/local/bin/i86-ar
	@install binpsx/i86-as /usr/local/bin/i86-as
	@install binpsx/i86-ld /usr/local/bin/i86-ld
	@install binpsx/i86-nm /usr/local/bin/i86-nm
	@install binpsx/i86-size /usr/local/bin/i86-size
	@install binpsx/i86-cc0 /usr/local/bin/i86-cc0
	@echo [INSTALL] /usr/local/lib/hcsystem
	@install -d /usr/local/lib/hcsystem
	@install lib/libc-dos.a /usr/local/lib/hcsystem/libc-dos.a
	@install lib/libc-hcs.a /usr/local/lib/hcsystem/libc-hcs.a

token:
	@nasm -f bin -o tests/token.bin tests/token.s
	@ndisasm -b 16 tests/token.bin > tests/token.dis