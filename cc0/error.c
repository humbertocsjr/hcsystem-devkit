#include "cc.h"

void error(char *fmt, ...)
{
    source_t *src;
    token_t *tok;
    va_list args;
    va_start(args, fmt);
    src = get_source();
    tok = get_token();
    if(src && tok)
    {
        fprintf(stderr, "%s:%d:%d: error: ", src->name, tok->line, tok->column);
    }
    else if(src)
    {
        fprintf(stderr, "%s:%d:%d: error: ", src->name, src->line, src->column);
    }
    else
    {
        fprintf(stderr, "error: ");
    }
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}


void error_at(uint16_t line, uint16_t column, char *fmt, ...)
{
    source_t *src;
    va_list args;
    va_start(args, fmt);
    src = get_source();
    if(src)
    {
        fprintf(stderr, "%s:%d:%d: error: ", src->name, line, column);    
    }
    else
    {
        fprintf(stderr, "%d:%d: error: ", line, column);
    }
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

void match(int cmp, char *expected)
{
    if(!cmp)error("%s expected.", expected);
    scan();
}

void match_at(uint16_t line, uint16_t column, int cmp, char *expected)
{
    if(!cmp)error_at(line, column, "%s expected.", expected);
    scan();
}
