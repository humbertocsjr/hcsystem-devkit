OUT = echo.com

all native: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT) *.o

%.o: %.s
	@echo [AS] $@
	@../binpsx/i86-as -o $@ $<

%.com: %.o
	@echo [LD] $@
	@../binpsx/i86-ld -f com -o $@ $<