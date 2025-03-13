AR = ../../binpsx/i86-ar
AS = ../../binpsx/i86-as
LD = ../../binpsx/i86-ld
SCC = ../../binpsx/i86-scc
OUT = ../libc-dos.a ../libc-hcs.a
SRCDOS = $(wildcard dos/*.asm) $(wildcard *.asm) $(wildcard dos/*.c) $(wildcard *.c)
OBJDOS = $(patsubst %.c,%.o,$(patsubst %.asm,%.o,$(SRCDOS)))
SRCHCSYS = $(wildcard hcsystem/*.asm) $(wildcard *.asm) $(wildcard hcsystem/*.c) $(wildcard *.c)
OBJHCSYS = $(patsubst %.c,%.o,$(patsubst %.asm,%.o,$(SRCHCSYS)))

native all: $(OUT)
	@echo >> /dev/null
	
clean:
	@rm -f $(OUT) *.o dos/*.o hcsystem/*.o

%.o: %.asm $(AR) $(LD) $(AS) Makefile
	@echo [AS] $@
	@$(AS) -o $@ $<

%.o: %.c $(AR) $(LD) $(AS) $(SCC) Makefile
	@echo [CC] $@
	@rm -f $(patsubst %.c,%.s,$<)
	@$(SCC) -i ../../include/ -S $<
	@$(AS) -o $@ $(patsubst %.c,%.s,$<)
	@rm -f $(patsubst %.c,%.s,$<)

../libc-dos.a: $(OBJDOS)
	@echo [AR] $@
	@rm -f $@
	@$(AR) -f $@ -a $^

../libc-hcs.a: $(OBJHCSYS)
	@echo [AR] $@
	@rm -f $@
	@$(AR) -f $@ -a $^