#ifndef DEVKIT_H
#define DEVKIT_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define VERSION 0
#define REVISION 92
#define EDITION "beta"
#define HOST_DOS "DOS"
#define HOST_POSIX "POSIX"
#define HOST_LINUX "Linux"
#define HOST_WINDOWS "Windows"
#ifndef HOST
    #define HOST "Unknown"
#endif

#pragma pack(1)

typedef enum segment_t
{
    SEGMENT_HEADER,
    SEGMENT_TEXT,
    SEGMENT_DATA,
    SEGMENT_BSS,
    SEGMENT_SYMBOLS
} segment_t;

typedef struct section_t
{
    struct section_t *next;
    segment_t segment;
    char * name;
    int32_t size;
} section_t;

enum
{
    // CS | DS=ES=SS
    AOUT_SMALL_V7 = 0411,
    // CS=DS=ES=SS
    AOUT_TINY_V7 = 0407,
    // CS | DS=ES=SS
    AOUT_HC_IX_EXEC = 0511,
    AOUT_HC_IX_OBJ = 0500
};

typedef struct aout_t
{
    uint16_t signature;
    uint16_t text;
    uint16_t data;
    uint16_t bss;
    uint16_t syms;
    uint16_t entry;
    uint16_t pad;
    uint16_t relocs;
} aout_t;

enum
{
    AOUT_MASK_OFFSET = 0xff,
    AOUT_MASK_SYMTYPE = 0x300,
    AOUT_MASK_SEG = 0xf000,
    AOUT_SYMTYPE_TEXTSEG = 0x1000,
    AOUT_SYMTYPE_DATASEG = 0x2000,
    AOUT_SYMTYPE_BSSSEG = 0x4000,
    AOUT_SYMTYPE_GLOBAL = 0x0100,
    AOUT_SYMTYPE_REFERENCE = 0x0200,
    AOUT_SYMTYPE_BYTE_OFFSET = 0x0400,
    AOUT_SYMTYPE_WORD_OFFSET = 0x0800
};

typedef struct aout_symbol_t
{
    char symbol[60];
    uint16_t type;
    uint16_t offset;
} aout_symbol_t;

enum
{
    AR_AOUT = 0711,
    AR_TEXT = 0721
};

typedef struct ar_t
{
    uint16_t signature;
    uint16_t size;
    char name[60];
} ar_t;


#pragma pack()

#endif
