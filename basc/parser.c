#include "basc.h"

typedef struct level_t
{
    enum
    {
        LEVEL_ROOT,
        LEVEL_SUB,
        LEVEL_FUNCTION,
        LEVEL_DO,
        LEVEL_IF
    } type;
    uint16_t continue_label;
    uint16_t exit_label;
} level_t;

static level_t _levels[LEVELS_MAX];
static int _levels_current = 0;
static uint16_t _label = 1;
static function_t *_function = 0;

uint16_t add_label()
{
    return _label++;
}

void remove_level(int type)
{
    if(_levels[_levels_current].type != type)
    {
        error("invalid level.");
    }
    _levels_current--;
}

level_t *add_level(int type, uint16_t continue_label, uint16_t exit_label)
{
    _levels_current++;
    if(_levels_current >= LEVELS_MAX)
    {
        error("levels list overflow.");
    }
    _levels[_levels_current].type = type;
    _levels[_levels_current].continue_label = continue_label;
    _levels[_levels_current].exit_label = exit_label;
    return &_levels[_levels_current];
}

void prepare_parser()
{
    _levels_current = 0;
    _levels[_levels_current].type = LEVEL_ROOT;
    _levels[_levels_current].continue_label = 0;
    _levels[_levels_current].exit_label = 0;
}

type_t parse_type()
{
    if(is_token(TOK_KEY_INTEGER))
    {
        scan();
        return TYPE_INTEGER;
    }
    if(is_token(TOK_KEY_STRING))
    {
        scan();
        return TYPE_STRING;
    }
    error("unknown data type");
    return 0;
}

void parse_routine(int name_export)
{
    int is_function = is_token(TOK_KEY_FUNCTION);
    char *name;
    scan();
    name = add_name(get_token()->text);
    match(TOK_SYMBOL, "name");
    add_level(is_function ? LEVEL_FUNCTION : LEVEL_SUB, add_label(), add_label());
    _function = add_function(name_export, 0, name, get_code_size());
    if(is_token(TOK_PARAMS_OPEN))
    {
        scan();
        match(TOK_PARAMS_CLOSE, "')'");
    }
    if(is_function)
    {
        match(TOK_KEY_AS, "'as'");
        _function->type = parse_type();
    }
    gen_routine(name, _function->type);
    if(_function->vars) gen_reserve_stack(-_function->vars_next);
}

void parse_end()
{
    match(TOK_KEY_END, "'end'");
    if(is_token(TOK_KEY_SUB))
    {
        scan();
        remove_level(LEVEL_SUB);
        gen_end_routine(_function->name, _function->type);
        _function = 0;
    }
    else if(is_token(TOK_KEY_FUNCTION))
    {
        scan();
        remove_level(LEVEL_FUNCTION);
        gen_end_routine(_function->name, _function->type);
        _function = 0;
    }
    else error("unknown 'end' type.");
}

type_t get_type(expr_t *e)
{
    var_t *v;
    function_t *f;
    if(e->tok == TOK_SYMBOL)
    {
        if(_function)
        {
            v = find_local_var(_function, e->text);
            if(v)
            {
                return v->type;
            }
        }
        v = find_global_var(e->text);
        if(v)
        {
            return v->type;
        }
        f = find_function(e->text);
        if(f)
        {
            return f->type;
        }
    }
    error("variable expected.");
    return 0;
}

void parse_expr_write_node(type_t type, expr_t *e)
{
    var_t *v;
    switch(e->tok)
    {
        case TOK_SYMBOL:
            v = find_local_var(_function, e->text);
            if(v)
            {
                gen_store_local(v->name, v->offset);
                return;
            }
            v = find_global_var(e->text);
            if(v)
            {
                gen_store_global(v->name, v->offset);
                return;
            }
            error_at(e->line, e->column, "variable expected.");
            break;
    }
    error_at(e->line, e->column, "valid destination expected.");
}

void parse_expr_common_node(type_t type, expr_t *e)
{
    extern void parse_expr_node(type_t type, expr_t *e);
    parse_expr_node(type, e->right);
    gen_push_acc(type);
    parse_expr_node(type, e->left);
    gen_pop_aux(type);
}

void parse_expr_node(type_t type, expr_t *e)
{
    var_t *v;
    if(e == 0) return;
    switch(e->tok)
    {
        case TOK_INTEGER:
            gen_set_acc(e->value);
            break;
        case TOK_SYMBOL:
            v = find_local_var(_function, e->text);
            if(v)
            {
                gen_load_local(v->name, v->offset);
                return;
            }
            v = find_global_var(e->text);
            if(v)
            {
                gen_load_global(v->name, v->offset);
                return;
            }
            error_at(e->line, e->column, "variable expected.");
            break;
        case TOK_ADD:
            if(e->right->tok == TOK_INTEGER)
            {
                parse_expr_node(type, e->left);
                gen_add_direct(type, e->right->value);
            }
            else if(e->left->tok == TOK_INTEGER)
            {
                parse_expr_node(type, e->right);
                gen_add_direct(type, e->left->value);
            }
            else
            {
                parse_expr_node(type, e->right);
                gen_push_acc(type);
                parse_expr_node(type, e->left);
                gen_pop_aux(type);
                gen_add(type);
            }
            break;
        case TOK_SUB:
            if(e->right->tok == TOK_INTEGER)
            {
                parse_expr_node(type, e->left);
                gen_sub_direct(type, e->right->value);
            }
            else
            {
                parse_expr_node(type, e->right);
                gen_push_acc(type);
                parse_expr_node(type, e->left);
                gen_pop_aux(type);
                gen_sub(type);
            }
            break;
        case TOK_MUL:
            parse_expr_common_node(type, e);
            gen_mul(type);
            break;
        case TOK_DIV:
            parse_expr_common_node(type, e);
            gen_div(type);
            break;
        case TOK_KEY_MOD:
            parse_expr_common_node(type, e);
            gen_mod(type);
            break;
        case TOK_KEY_SHL:
            parse_expr_common_node(type, e);
            gen_shl(type);
            break;
        case TOK_KEY_SHR:
            parse_expr_common_node(type, e);
            gen_shr(type);
            break;
        case TOK_ATTRIB:
            if(e->left->tok == TOK_SYMBOL && e->left->left == 0 && e->left->right == 0 && e->right->tok == TOK_INTEGER)
            {
                v = find_local_var(_function, e->left->text);
                if(v)
                {
                    gen_store_local_direct(v->name, v->offset, e->right->value);
                    return;
                }
                v = find_global_var(e->left->text);
                if(v)
                {
                    gen_store_global_direct(v->name, v->offset, e->right->value);
                    return;
                }
                error_at(e->line, e->column, "variable expected.");
            }
            else
            {
                parse_expr_node(get_type(e->left), e->right);
                parse_expr_write_node(get_type(e->left), e->left);
            }
            break;
    }
}

void parse_expr(type_t type)
{
    expr_t *e = optimize(type, expr(type));
    if(e->tok == TOK_EQ)
    {
        e->tok = TOK_ATTRIB;
    }
    parse_expr_node(type, e);
}

void parse_dim(int name_export)
{
    type_t type;
    expr_t *e;
    char *name;
    uint16_t array_size = 0;
    int attrib = 0;
    match(TOK_KEY_DIM, "dim");
    do
    {
        attrib = 0;
        name = add_name(get_token()->text);
        match(TOK_SYMBOL, "name");
        if(is_token(TOK_PARAMS_OPEN))
        {
            scan();
            match(TOK_PARAMS_CLOSE, "')'");
        }
        match(TOK_KEY_AS, "'as'");
        type = parse_type();
        if(is_token(TOK_EQ))
        {
            e = add_expr_from_token(get_token(), type);
            scan();
            e->tok = TOK_ATTRIB;
            e->left = add_expr(TOK_SYMBOL, type, e->line, e->column, name);
            e->right = expr(type);
            if(get_stage() != STAGE_CATALOG)  attrib = 1;
        }
        if(_levels[_levels_current].type == LEVEL_ROOT)
        {
            add_global_var(name_export, name, type, array_size, get_data_size());
            if(type & TYPE_INTEGER)
            {
                if(attrib)
                {
                    e = optimize(type, e);
                    if(e->right->tok != TOK_INTEGER) error_at(e->line, e->column, "constant expression expected.");
                    gen_global_integer(name, e->right->value);
                }
                else
                {
                    gen_global_integer(name, 0);
                }
            }
            if(type & TYPE_STRING)
            {
                if(attrib)
                {
                    e = optimize(type, e);
                    if(e->right->tok != TOK_STRING) error_at(e->line, e->column, "constant expression expected.");
                    gen_global_string(name, strlen(e->right->text) + 1, e->right->text);
                }
                else
                {
                    gen_global_string(name, 2, "");
                }
            }
        }
        else
        {
            add_local_var(_function, name, type, array_size);
            if(attrib) parse_expr_node(type, e);
        }
        if(!is_token(TOK_COMMA)) break;
        scan();
    } while(!is_token(TOK_NEW_LINE));
}

void parse()
{
    int name_export = 0;
    reset_exprs();
    if(_levels[_levels_current].type == LEVEL_ROOT)
    {
        if(is_token(TOK_KEY_PUBLIC))
        {
            scan();
            name_export = 1;
        }
        else if(is_token(TOK_KEY_PRIVATE))
        {
            scan();
            name_export = 0;
        }
        if(is_token(TOK_KEY_DIM))
        {
            parse_dim(name_export);
        }
        else if(is_token(TOK_KEY_SUB) || is_token(TOK_KEY_FUNCTION))
        {
            parse_routine(name_export);
        }
        else if(is_token(TOK_NEW_LINE))
        {
            scan();
            return;
        }
        else error("command unknown.");
    }
    else
    {
        if(is_token(TOK_KEY_DIM))
        {
            parse_dim(name_export);
        }
        else if(is_token(TOK_KEY_END))
        {
            parse_end();
        }
        else if(is_token(TOK_SYMBOL) && get_stage() == STAGE_CATALOG)
        {
            expr(TYPE_INTEGER);
        }
        else if(is_token(TOK_SYMBOL))
        {
            parse_expr(TYPE_INTEGER);
        }
        else if(is_token(TOK_NEW_LINE))
        {
            scan();
            return;
        }
        else error("command unknown.");
    }
    match(TOK_NEW_LINE, "new line");
}
