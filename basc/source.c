#include "basc.h"

static source_t *_source = 0;
static token_t _current;
static token_t _peek;

source_t *get_source()
{
    return _source;
}

int char_is(char c)
{
    return _source->c == c;
}

int char_is_between(char min, char max)
{
    return tolower(min) <= tolower(_source->c) && tolower(_source->c) <= tolower(max);
}

char get_char()
{
    return _source->c;
}

char get_lower_char()
{
    return tolower(_source->c);
}

char read_char()
{
    _source->c = fgetc(_source->file);
    _source->column++;
    if(_source->c == EOF)
    {
        _source->line++;
        _source->column = 0;
        _source->c = 0;
    }
    if(_source->c == '\n')
    {
        _source->line++;
        _source->column = 0;
    }
    if(_source->c == '\t')
    {
        _source->column += 3;
    }
    if(_source->c == '\r')
    {
        _source->column = 0;
    }
    return _source->c;
}

void process_source(char *file)
{
    source_t *old = _source;
    source_t src;
    _source = &src;
    src.file = fopen(file, "r");
    if(!src.file)
    {
        _source = old;
        error("can't open file: %s", file);
    }
    src.name = add_name(file);
    src.line = 1;
    src.column = 0;
    src.c = 0;
    read_char();
    scan();
    scan();
    while(!is_token(TOK_EOF)) 
    {
        parse();
    }
    fclose(src.file);
    _source = old;
}

token_t *get_token()
{
    return &_current;
}

token_t *peek_token()
{
    return &_peek;
}

static void _concat_peek(char c)
{
    char tmp[2];
    tmp[0] = c;
    tmp[1] = 0;
    if((strlen(_peek.text) + 1) >= TOKEN_MAX)
    {
        error_at(_source->line, _source->column, "token size overflow: %s", _peek.text);
    }
    strlcat(_peek.text, tmp, TOKEN_MAX);
}

token_t *scan()
{
    memcpy(&_current, &_peek, sizeof(token_t));
    memset(&_peek, 0, sizeof(token_t));
    _peek.tok = TOK_EOF;
    while(char_is(' ') || char_is('\t') || char_is('\r')) read_char();
    _peek.line = _source->line;
    _peek.column = _source->column;
    if(char_is(0) && _current.tok == TOK_NEW_LINE) return get_token();
    if(char_is(0))
    {
        _peek.tok = TOK_NEW_LINE;
        return get_token();
    }
    if(char_is_between('0', '9'))
    {
        _peek.tok = TOK_INTEGER;
        while(char_is_between('0', '9'))
        {
            _concat_peek(get_char());
            read_char();
        }
    }
    else if(char_is_between('a', 'z') || char_is('_'))
    {
        _peek.tok = TOK_SYMBOL;
        while(char_is_between('a', 'z') || char_is_between('0', '9') || char_is('.') || char_is('_'))
        {
            _concat_peek(get_lower_char());
            read_char();
        }
        if(!strcmp(_peek.text, "mod")) _peek.tok = TOK_KEY_MOD;
        if(!strcmp(_peek.text, "shl")) _peek.tok = TOK_KEY_SHL;
        if(!strcmp(_peek.text, "shr")) _peek.tok = TOK_KEY_SHR;
        if(!strcmp(_peek.text, "if")) _peek.tok = TOK_KEY_IF;
        if(!strcmp(_peek.text, "else")) _peek.tok = TOK_KEY_ELSE;
        if(!strcmp(_peek.text, "then")) _peek.tok = TOK_KEY_THEN;
        if(!strcmp(_peek.text, "while")) _peek.tok = TOK_KEY_WHILE;
        if(!strcmp(_peek.text, "do")) _peek.tok = TOK_KEY_DO;
        if(!strcmp(_peek.text, "end")) _peek.tok = TOK_KEY_END;
        if(!strcmp(_peek.text, "until")) _peek.tok = TOK_KEY_UNTIL;
        if(!strcmp(_peek.text, "for")) _peek.tok = TOK_KEY_FOR;
        if(!strcmp(_peek.text, "to")) _peek.tok = TOK_KEY_TO;
        if(!strcmp(_peek.text, "step")) _peek.tok = TOK_KEY_STEP;
        if(!strcmp(_peek.text, "sub")) _peek.tok = TOK_KEY_SUB;
        if(!strcmp(_peek.text, "function")) _peek.tok = TOK_KEY_FUNCTION;
        if(!strcmp(_peek.text, "declare")) _peek.tok = TOK_KEY_DECLARE;
        if(!strcmp(_peek.text, "dim")) _peek.tok = TOK_KEY_DIM;
        if(!strcmp(_peek.text, "as")) _peek.tok = TOK_KEY_AS;
        if(!strcmp(_peek.text, "exit")) _peek.tok = TOK_KEY_EXIT;
        if(!strcmp(_peek.text, "continue")) _peek.tok = TOK_KEY_CONTINUE;
        if(!strcmp(_peek.text, "integer")) _peek.tok = TOK_KEY_INTEGER;
        if(!strcmp(_peek.text, "string")) _peek.tok = TOK_KEY_STRING;
        if(!strcmp(_peek.text, "public")) _peek.tok = TOK_KEY_PUBLIC;
        if(!strcmp(_peek.text, "private")) _peek.tok = TOK_KEY_PRIVATE;
        if(_peek.tok != TOK_SYMBOL) strcpy(_peek.text, "");
    }
    else if(char_is('"'))
    {
        read_char();
        _peek.tok = TOK_STRING;
        while(!char_is('"'))
        {
            _concat_peek(get_char());
            read_char();
        }
        if(!char_is('"')) error_at(_source->line, _source->column, "'\"' expected.");
        read_char();
    }
    else if(char_is(','))
    {
        _peek.tok = TOK_COMMA;
        read_char();
    }
    else if(char_is('('))
    {
        _peek.tok = TOK_PARAMS_OPEN;
        read_char();
    }
    else if(char_is(')'))
    {
        _peek.tok = TOK_PARAMS_CLOSE;
        read_char();
    }
    else if(char_is('+'))
    {
        _peek.tok = TOK_ADD;
        read_char();
    }
    else if(char_is('-'))
    {
        _peek.tok = TOK_SUB;
        read_char();
    }
    else if(char_is('/'))
    {
        _peek.tok = TOK_DIV;
        read_char();
    }
    else if(char_is('*'))
    {
        _peek.tok = TOK_MUL;
        read_char();
    }
    else if(char_is('='))
    {
        _peek.tok = TOK_EQ;
        read_char();
    }
    else if(char_is('<'))
    {
        _peek.tok = TOK_LT;
        read_char();
        if(char_is('>'))
        {
            _peek.tok = TOK_NE;
            read_char();
        }
        else if(char_is('='))
        {
            _peek.tok = TOK_LE;
            read_char();
        }
    }
    else if(char_is('>'))
    {
        _peek.tok = TOK_GT;
        read_char();
        if(char_is('='))
        {
            _peek.tok = TOK_GE;
            read_char();
        }
    }
    else if(char_is('\n'))
    {
        _peek.tok = TOK_NEW_LINE;
        read_char();
    }
    else error_at(_peek.line, _peek.column, "token unknown: '%c'", get_char());
    return get_token();
}

int is_token(tok_t tok)
{
    return _current.tok == tok;
}

int is_peek(tok_t tok)
{
    return _peek.tok == tok;
}
