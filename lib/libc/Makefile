AR = ../../binpsx/i86-ar
AS = ../../binpsx/i86-as
LD = ../../binpsx/i86-ld
OUT = ../libc-dos.a ../libc-hcs.a
SRCDOS = $(wildcard dos/*.s) $(wildcard *.s)
OBJDOS = $(patsubst %.s,%.o,$(SRCDOS))
SRCHCSYS = $(wildcard dos/*.s) $(wildcard *.s)
OBJHCSYS = $(patsubst %.s,%.o,$(SRCHCSYS))

all: $(OUT)
	@echo >> /dev/null
clean:
	@rm -f $(OUT) *.o dos/*.o hcsystem/*.o

%.o: %.s $(AR) $(LD) $(AS)
	@echo [AS] $@
	@$(AS) -o $@ $<

../libc-dos.a: $(OBJDOS)
	@echo [AR] $@
	@$(AR) -f $@ -a $^

../libc-hcs.a: $(OBJHCSYS)
	@echo [AR] $@
	@$(AR) -f $@ -a $^