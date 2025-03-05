# HCSystem Development Kit

DevKit focused on retrocomputers.

# Development Reqs

- GNU Make
- OpenWatcom 2.0
- dev86/bcc (tests only)

# Targets

- i86: Intel 8088/8086 or Compatible

# Binary Formats

- v7: UNIX V7 a.out code and data in separated segments
- v7tiny: UNIX V7 a.out with code and data in one segment
- hcix: HC-IX a.out
- com: DOS COM file
- sys: DOS SYS file
- pcboot: PC Boot Sector

# Tools:

- ar: Archiver
- as: Assembler
- ld: Linker
- size: Object Size

# Assembler

```sh
i86-as -o test.o test.s
```

# Linker

```sh
i86-ld -f dos -o test.com test.o
```
