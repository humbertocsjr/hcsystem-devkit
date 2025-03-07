# HCSystem Development Kit

DevKit focused on retrocomputers.

# Development Reqs

- GNU Make
- OpenWatcom 2.0
- nasm (tests only)
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
- nm: Symbol Names
- size: Object Size
- basc: BASIC Compiler

# i86-as: Assembler

```
HCSystem Software Development Kit for POSIX
HCSystem i86 Assembler v0.91-beta
Copyright (c) 2025, Humberto Costa dos Santos Junior

Usage: as [-o output] [objects...]
Options:
 -o output       set output file
```

# i86-ar: Archiver

```
HCSystem Software Development Kit for POSIX
HCSystem Archiver v0.91-beta
Copyright (c) 2025, Humberto Costa dos Santos Junior

Usage: ar [-f archive] [-a file] [-l]
Options:
 -f archive      set archive file
 -a file         append file to archive
 -l              list all files from archive
```

# i86-ld: Linker

```
HCSystem Software Development Kit for POSIX
HCSystem Linker v0.91-beta
Copyright (c) 2025, Humberto Costa dos Santos Junior

Usage: ld [-f format] [-b] [-o output] [-L dir] [-l library] [-s stack_size] [objects...]
Options:
 -f format       set output format (see list bellow)
 -b              include bss space to output
 -o output       set output file
 -s stack_size   set stack size (default: 2048)
 -L dir          add to library dirs list
 -l library      add library (auto include 'lib' prefix)
Output format:
 hcix            produce HC-IX a.out
 hcsys           produce HCSystem a.out
 v7|v7small      produce Seventh Edition UNIX a.out
 v7tiny          tiny model output (text/data/bss in one segment)
 com             DOS COM executable (ORG 0x100)
 sys             DOS SYS executable (ORG 0x0)
 pcboot          PC Boot Sector executable (ORG 0x7c00)
```

# i86-size: Object size

```
HCSystem Software Development Kit for POSIX
HCSystem Object Size v0.91-beta
Copyright (c) 2025, Humberto Costa dos Santos Junior

Usage: size [objects...]
```

# i86-nm: Symbol names

```
HCSystem Software Development Kit for POSIX
HCSystem Symbol Names v0.91-beta
Copyright (c) 2025, Humberto Costa dos Santos Junior

Usage: nm [objects...]
```
# i86-basc: BASIC Compiler

```
HCSystem Software Development Kit for POSIX
HCSystem BASIC Compiler v0.91-beta
Copyright (c) 2025, Humberto Costa dos Santos Junior

Usage: basc [-o output] [sources]
Options:
 -o output       set output file
```