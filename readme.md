# HCSystem Software Development Kit

DevKit focused on retrocomputers.

## Targets

- i86: Intel 8088/8086 or Compatible

## Supported Hosts

- Linux x86
- Windows x86
- DOS 8086

## Binary Formats

- v7: UNIX V7 a.out code and data in separated segments
- v7tiny: UNIX V7 a.out with code and data in one segment
- hcix: HC-IX a.out
- com: DOS COM file
- sys: DOS SYS file
- pcboot: PC Boot Sector

## Tools:

- ar: Archiver
- as: Assembler
- ld: Linker
- nm: Symbol Names
- size: Object Size
- cc0: C Compiler

## Usage

- Assemble source to DOS .COM
```sh
i86-as -o test.o test.s
i86-ld -f com -o test.com test.o
```

- Compile C source to DOS .COM [DON'T USE. IN DEVELOPMENT]
```sh
i86-cc0 -o test.s test.c
i86-as -o test.o test.s
i86-ld -f com -o test.com -L /usr/local/lib/hcsystem -l libdos.a test.o
```

- Assemble multiple sources to Library
```sh
i86-as -o test0.o test0.s
i86-as -o test1.o test1.s
i86-ar -f libtest.a -a test0.o test1.o
```

- Assemble multiple sources with Library
```sh
i86-as -o test2.o test2.s
i86-as -o test3.o test3.s
i86-ld -f com -o test.com -L /libs/path/ -l libtest.a test2.o test3.o
```

## Development Reqs

- GNU Make
- OpenWatcom 2.0
- nasm (tests only)
- dev86/bcc (tests only)

## Versions Roadmap

- [ ] 2.00-final
    - [ ] Add i80 Target
    - [ ] Add z80 Target
    - [ ] Add Assembly Macro Processor

- [ ] 1.00-final
    - [ ] Add Far pointer

- [ ] 0.94-beta
    - [ ] Add b2o tool (binary to object converter)

- [ ] 0.93-beta
    - [ ] Add Minimal DOS Library
        - [ ] Add _start routine
    - [ ] Add C Pre-processor

- [ ] 0.92-beta
    - [x] Add C Compiler minimal prototype

- [x] 0.91-beta
    - [x] Add missing opcodes to Assembler (eg.: cmp REG16, VALUE)
    - [x] Add UNIX NM-like Tool
    - [x] Add NASM dot labels to Assembler
    - [x] Add ECHO-like example to distribution

- [x] 0.90-beta
    - [x] Add PCBOOT File format
    - [x] Add UNIX Size-like tool
    - [x] Add Archiver tool

- [x] 0.02-alpha
    - [x] Add NASM TIMES command to Assembler
    - [x] Add DOS COM/SYS File format

- [x] 0.01-alpha
    - [x] Add Linker tool
    - [x] Add 8086 Assembler tool

# Assembly Language

Based on NASM language.

- [x] Labels
    ```nasm
    main:
        .test:
        .end:
    ```
- [x] 8086 Opcodes
    ```nasm
    mov word [0x123], 0x456
    ```
- [x] .text/.data/.bss sections
    ```nasm
    section .text
    section .data
    section .bss
    ```
- [x] db/dw/dd command
    ```nasm
    db 0x11
    dw 0x2222
    dd 0x44444444
    ```
- [x] resb/resw/resd command
    ```nasm
    resb 5
    resw 1
    resd 3
    ```
- [x] times-like command
    ```nasm
    times 5 db 5
    ```

# C Language

- [x] global variables
- [x] do/while/for loops
- [x] pointers

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
# i86-cc0: C Compiler

```
HCSystem Software Development Kit for POSIX
HCSystem C Compiler v0.91-beta
Copyright (c) 2025, Humberto Costa dos Santos Junior

Usage: cc0 [-o output] [sources]
Options:
 -o output       set output file
```
