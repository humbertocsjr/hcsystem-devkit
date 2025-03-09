#include "cc.h"

static FILE *_out = 0;

int is_out_enabled()
{
    return _out != 0;
}

void open_out(char *file)
{
    unlink(file);
    _out = fopen(file, "w");
    if(!_out) error("can't create file: %s", file);
}

void close_out()
{
    fclose(_out);
}

void out(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if(_out) 
    {
        vfprintf(_out, fmt, args);
        fprintf(_out, "\n");
    }
    va_end(args);
}
