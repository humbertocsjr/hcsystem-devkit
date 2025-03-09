#include "cc.h"

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
    while(char_is(' ') || char_is('\t') || char_is('\r') || char_is('\n')) read_char();
    _peek.line = _source->line;
    _peek.column = _source->column;
    if(char_is(0) && _current.tok == TOK_SEMI) return get_token();
    if(char_is(0))
    {
        _peek.tok = TOK_SEMI;
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
        if(!strcmp(_peek.text, "if")) _peek.tok = TOK_KEY_IF;
        if(!strcmp(_peek.text, "else")) _peek.tok = TOK_KEY_ELSE;
        if(!strcmp(_peek.text, "while")) _peek.tok = TOK_KEY_WHILE;
        if(!strcmp(_peek.text, "do")) _peek.tok = TOK_KEY_DO;
        if(!strcmp(_peek.text, "for")) _peek.tok = TOK_KEY_FOR;
        if(!strcmp(_peek.text, "continue")) _peek.tok = TOK_KEY_CONTINUE;
        if(!strcmp(_peek.text, "int")) _peek.tok = TOK_KEY_INT;
        if(!strcmp(_peek.text, "short")) _peek.tok = TOK_KEY_INT;
        if(!strcmp(_peek.text, "char")) _peek.tok = TOK_KEY_CHAR;
        if(!strcmp(_peek.text, "signed")) _peek.tok = TOK_KEY_SIGNED;
        if(!strcmp(_peek.text, "unsigned")) _peek.tok = TOK_KEY_UNSIGNED;
        if(!strcmp(_peek.text, "void")) _peek.tok = TOK_KEY_VOID;
        if(!strcmp(_peek.text, "switch")) _peek.tok = TOK_KEY_SWITCH;
        if(!strcmp(_peek.text, "case")) _peek.tok = TOK_KEY_CASE;
        if(!strcmp(_peek.text, "break")) _peek.tok = TOK_KEY_BREAK;
        if(!strcmp(_peek.text, "goto")) _peek.tok = TOK_KEY_GOTO;
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
    else if(char_is('['))
    {
        _peek.tok = TOK_INDEX_OPEN;
        read_char();
    }
    else if(char_is(']'))
    {
        _peek.tok = TOK_INDEX_CLOSE;
        read_char();
    }
    else if(char_is('{'))
    {
        _peek.tok = TOK_BLOCK_OPEN;
        read_char();
    }
    else if(char_is('}'))
    {
        _peek.tok = TOK_BLOCK_CLOSE;
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
    else if(char_is('&'))
    {
        _peek.tok = TOK_AND;
        read_char();
        if(char_is('&'))
        {
            _peek.tok = TOK_AND_ALSO;
            read_char();
        }
    }
    else if(char_is('='))
    {
        _peek.tok = TOK_ATTRIB;
        read_char();
        if(char_is('='))
        {
            _peek.tok = TOK_EQ;
            read_char();
        }
    }
    else if(char_is('!'))
    {
        _peek.tok = TOK_NOT;
        read_char();
        if(char_is('='))
        {
            _peek.tok = TOK_NE;
            read_char();
        }
    }
    else if(char_is('<'))
    {
        _peek.tok = TOK_LT;
        read_char();
        if(char_is('<'))
        {
            _peek.tok = TOK_SHL;
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
        if(char_is('>'))
        {
            _peek.tok = TOK_SHR;
            read_char();
        }
        else if(char_is('='))
        {
            _peek.tok = TOK_GE;
            read_char();
        }
    }
    else if(char_is('%'))
    {
        _peek.tok = TOK_MOD;
        read_char();
    }
    else if(char_is(':'))
    {
        _peek.tok = TOK_COLON;
        read_char();
    }
    else if(char_is(';'))
    {
        _peek.tok = TOK_SEMI;
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
