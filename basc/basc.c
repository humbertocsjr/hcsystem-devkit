#include "../include/devkit.h"
#include "basc.h"

void usage(int retval)
{
    printf("HCSystem Software Development Kit for %s\n", HOST);
    printf("HCSystem BASIC Compiler v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: basc [-o output] [sources]\n");
    printf("Options:\n");
    printf(" -o output       set output file\n");
    exit(retval);
}

int main(int argc, char **argv)
{
    int c, i;
    FILE *out;
    FILE *in;
    char *out_name = "a.out";
    aout_t header;
    if(argc <= 1) usage(1);
    while((c = getopt(argc, argv, "ho:a:l")) != -1)
    {
        switch(c)
        {
            case 'o':
                out_name = optarg;
                break;
            case 'h':
                usage(0);
                return -1;
        }
    }
    prepare_parser();
    open_out(out_name);
    header.signature = AOUT_HC_IX_OBJ;
    header.text = 0;
    header.data = 0;
    header.bss = 0;
    header.syms = 0;
    header.entry = 0;
    header.pad = 0;
    header.relocs = 0;
    out_header(&header);
    set_stage(STAGE_CATALOG);
    for(i = optind; i < argc; i++)
    {
        process_source(argv[i]);
    }
    set_stage(STAGE_CODE_GEN);
    for(i = optind; i < argc; i++)
    {
        process_source(argv[i]);
    }
    set_stage(STAGE_STRING_GEN);
    for(i = optind; i < argc; i++)
    {
        process_source(argv[i]);
    }
    set_stage(STAGE_DATA_GEN);
    for(i = optind; i < argc; i++)
    {
        process_source(argv[i]);
    }
    header.text = get_code_size();
    header.data = get_data_size();
    set_stage(STAGE_SYMBOL_GEN);
    header.syms = generate_symbols();
    out_header(&header);
    close_out();
    return 0;
}
