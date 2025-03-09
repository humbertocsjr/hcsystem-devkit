#include "cc.h"

static var_t _globals[GLOBALS_MAX];
static int _globals_next = 0;
static function_t _functions[FUNCTIONS_MAX];
static int _functions_next = 0;
static var_t _locals[LOCALS_MAX];
static int _locals_next = 0;
static int _label = 1;

int new_label()
{
    return _label++;
}

function_t *add_function(int name_export, dtype_t type, char *name)
{
    int i;
    function_t *func = &_functions[_functions_next];
    _functions_next++;
    if(_functions_next >= FUNCTIONS_MAX) error("function list overflow.");
    func->name = add_name(name);
    func->name_export = name_export;
    func->type = type;
    func->args = 0;
    func->vars = 0;
    func->args_next = get_args_offset();
    func->vars_next = get_vars_offset();
    return func;
}

var_t *add_global_var(int name_export, char *name, dtype_t type, uint16_t array_size)
{
    int i;
    var_t *v = &_globals[_globals_next];
    _globals_next++;
    if(_globals_next >= GLOBALS_MAX)
    {
        error("global variable list overflow: %s", name);
    }
    v->name = add_name(name);
    v->type = type;
    v->array_size = array_size;
    v->name_export = name_export;
    return v;
}

var_t *add_local_var(function_t *func, char *name, dtype_t type, uint16_t array_size)
{
    int i;
    var_t *v = func->vars;
    int32_t size = (array_size == 0 ? 1 : array_size) * get_size(type);
    if(func == 0) error("internal error. function expected.");
    v = &_locals[_locals_next];
    _locals_next++;
    if(_locals_next >= LOCALS_MAX)
    {
        error("local variable list overflow: %s", name);
    }
    v->name = add_name(name);
    v->type = type;
    v->array_size = array_size;
    func->vars_next -= size;
    v->offset = func->vars_next;
    v->name_export = 0;
    v->next = func->vars;
    func->vars = v;
    return v;
}

var_t *find_global_var(char *name)
{
    int i;
    for(i = 0; i < _globals_next; i++)
    {
        if(!strcmp(_globals[i].name, name))
        {
            return &_globals[i];
        }
    }
    return 0;
}

function_t *find_function(char *name)
{
    int i;
    for(i = 0; i < _functions_next; i++)
    {
        if(!strcmp(_functions[i].name, name))
        {
            return &_functions[i];
        }
    }
    return 0;
}

var_t *find_local_var(function_t *func, char *name)
{
    var_t *v = func->vars;
    while(v)
    {
        if(!strcmp(v->name, name))
        {
            return v;
        }
        v = v->next;
    }
    v = func->args;
    while(v)
    {
        if(!strcmp(v->name, name))
        {
            return v;
        }
        v = v->next;
    }
    return 0;
}

