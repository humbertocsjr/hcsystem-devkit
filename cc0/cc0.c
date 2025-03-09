#include "cc.h"

void usage(int retval)
{
    printf("HCSystem Software Development Kit for %s\n", HOST);
    printf("HCSystem C Compiler v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: cc0 [-o output] [sources]\n");
    printf("Options:\n");
    printf(" -o output       set output file\n");
    exit(retval);
}

int main(int argc, char **argv)
{
    int c, i;
    char *out_name = "a.out";
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
    for(i = optind; i < argc; i++)
    {
        process_source(argv[i]);
    }
    open_out(out_name);
    gen_header();
    for(i = optind; i < argc; i++)
    {
        process_source(argv[i]);
    }
    gen_footer();
    close_out();
    return 0;
}
