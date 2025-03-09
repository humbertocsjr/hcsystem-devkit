#include "cc.h"

static typedef_t _typedefs[TYPEDEFS_MAX];
static int _typedefs_next = 0;

int is_dtype(token_t *t)
{
    switch (t->tok) 
    {
        case TOK_KEY_CHAR:
        case TOK_KEY_INT:
        case TOK_KEY_SIGNED:
        case TOK_KEY_UNSIGNED:
        case TOK_KEY_VOID:
            return 1;
        default:
            return 0;
    }
}

dtype_t add_typedef(char *name, dtype_t dt)
{
    typedef_t *td = &_typedefs[_typedefs_next++];
    if(_typedefs_next >= TYPEDEFS_MAX) error("typedef list overflow.");
    td->name = add_name(name);
    td->dt = dt;
    return dt;
}

dtype_t get_typedef(char *name)
{
    int i;
    for(i = 0; i < _typedefs_next; i++)
    {
        if(!strcmp(_typedefs[i].name, name))
        {
            return _typedefs[i].dt;
        }
    }
    return DTYPE_UNKNOWN;;
}
