#include "../include/devkit.h"


void usage()
{
    printf("HCSystem Development Kit\n");
    printf("HCSystem Object Size v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: size [objects...]\n");
    exit(1);
}

int main(int argc, char **argv)
{
    aout_t aout;
    FILE *file;
    int i;
    if(argc <= 1) usage();
    for(i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-h"))
        {
            usage();
        }
    }
    printf("text\tdata\tbss\tdec\thex\tfilename\n");
    for(i = 1; i < argc; i++)
    {
        if((file = fopen(argv[1], "rb")))
        {
            fread(&aout, 1, sizeof(aout_t), file);
            if
            (
                aout.signature == AOUT_HC_IX_EXEC ||
                aout.signature == AOUT_HC_IX_OBJ ||
                aout.signature == AOUT_TINY_V7 ||
                aout.signature == AOUT_SMALL_V7
            )
            printf
            (
                "%d\t%d\t%d\t%d\t%x\t%s\n",
                aout.text,
                aout.data,
                aout.bss,
                aout.text+aout.data+aout.bss,
                aout.text+aout.data+aout.bss,
                argv[i]
            );
        }
    }
    return 0;
}
