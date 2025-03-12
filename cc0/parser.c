#include "cc.h"

static function_t *_function = 0;
static int _case_label = 0;
static int _break_label = 0;
static int _continue_label = 0;
static int _first_case = 0;
void parse_expr(dtype_t dt, expr_t *e);

dtype_t parse_dtype()
{
    if(is_token(TOK_KEY_SIGNED))
    {
        scan();
        if(is_token(TOK_KEY_CHAR))
        {
            scan();
            return DTYPE_CHAR | DTYPE_SIGNED;
        }
        if(is_token(TOK_KEY_INT))
        {
            scan();
            return DTYPE_INT | DTYPE_SIGNED;
        }
        return DTYPE_INT | DTYPE_SIGNED;
    }
    if(is_token(TOK_KEY_UNSIGNED))
    {
        scan();
        if(is_token(TOK_KEY_CHAR))
        {
            scan();
            return DTYPE_CHAR | DTYPE_UNSIGNED;
        }
        if(is_token(TOK_KEY_INT))
        {
            scan();
            return DTYPE_INT | DTYPE_UNSIGNED;
        }
        return DTYPE_INT | DTYPE_UNSIGNED;
    }
    if(is_token(TOK_KEY_CHAR))
    {
        scan();
        return DTYPE_CHAR | DTYPE_SIGNED;
    }
    if(is_token(TOK_KEY_INT))
    {
        scan();
        return DTYPE_INT | DTYPE_SIGNED;
    }
    if(is_token(TOK_KEY_VOID))
    {
        scan();
        return DTYPE_VOID;
    }
    match(0, "data type");
    return DTYPE_UNKNOWN;
}

dtype_t get_dtype_expr(expr_t *e)
{
    var_t *v;
    switch(e->tok)
    {
        case TOK_SYMBOL:
            if((v = find_global_var(e->text)))
            {
                return v->type;
            }
            break;
        default:
            error_at(e->line, e->column, "invalid write expression.");
            break;
    }
    return DTYPE_UNKNOWN;
}

void parse_index_expr(expr_t *e, var_t *v)
{
    if(e->left->tok == TOK_INTEGER)
    {
        gen_add_direct(get_size(v->type) * e->left->value);
    }
    else if(get_size(v->type) == 1)
    {
        gen_push_acc();
        parse_expr(DTYPE_INT | DTYPE_UNSIGNED, e->left);
    }
    else
    {
        gen_push_acc();
        parse_expr(DTYPE_INT | DTYPE_UNSIGNED, e->left);
        gen_set_aux(get_size(v->type));
        gen_umul();
        gen_pop_aux();
        gen_add();
    }
}

void parse_address_expr(dtype_t dt, expr_t *e)
{
    var_t *v;
    switch(e->tok)
    {
        case TOK_SYMBOL:
            if((v = find_global_var(e->text)))
            {
                gen_set_acc_global(v->name);
            }
            else error_at(e->line, e->column, "not implemented");
            break;
        default:
            error_at(e->line, e->column, "invalid write expression.");
            break;
    }
}

void parse_write_expr(dtype_t dt, expr_t *e)
{
    var_t *v;
    switch(e->tok)
    {
        case TOK_SYMBOL:
            if((v = find_global_var(e->text)))
            {
                cast_dtype(dt, v->type);
                gen_store_global(v->type, v->name);
            }
            else error_at(e->line, e->column, "not implemented");
            break;
        default:
            error_at(e->line, e->column, "invalid write expression.");
            break;
    }
}

void parse_common_expr(dtype_t dt, expr_t *e, int can_optimize_left)
{
    void parse_expr(dtype_t dt, expr_t *e);
    if(can_optimize_left && e->left->tok == TOK_INTEGER)
    {
        parse_expr(dt, e->right);
        gen_set_aux(e->left->value);
    }
    else if(e->right->tok == TOK_INTEGER)
    {
        parse_expr(dt, e->left);
        gen_set_aux(e->right->value);
    }
    else
    {
        parse_expr(dt, e->right);
        gen_push_acc();
        parse_expr(dt, e->left);
        gen_pop_aux();
    }
}

void parse_expr(dtype_t dt, expr_t *e)
{
    var_t *v;
    expr_t *arg;
    function_t *fn;
    dtype_t sub_dt;
    int size;
    if(!e) return;
    switch(e->tok)
    {
        case TOK_ADDRESSOF:
            parse_address_expr(dt, e->right);
            break;
        case TOK_POINTER:
            if(e->right->tok == TOK_SYMBOL && e->right->left == 0 && e->right->right == 0)
            {
                if((v = find_global_var(e->right->text)))
                {
                    gen_load_ref_global(ref_dtype(dt), v->name);
                }
                else error_at(e->line, e->column, "not implemented");
            }
            else
            {
                parse_expr(dt, e->right);
                gen_load(deref_dtype(dt));
            }
            break;
        case TOK_COMMA:
            parse_expr(DTYPE_INT | DTYPE_INT, e->right);
            gen_push_acc();
            parse_expr(DTYPE_INT | DTYPE_INT, e->left);
            break;
        case TOK_SYMBOL:
            if((v = find_local_var(_function, e->text)))
            {
                if(e->right)
                {
                    parse_expr(DTYPE_INT | DTYPE_INT, e->right);
                    gen_push_acc();
                }
                if(e->left)
                {
                    gen_set_acc_local(v->name, v->offset);
                    parse_index_expr(e, v);
                    gen_load(v->type);
                    cast_dtype(v->type, dt);
                }
                else
                {
                    gen_load_local(v->type, v->name, v->offset);
                    cast_dtype(v->type, dt);
                }
                if(e->right)
                {
                    gen_call_acc();
                    arg = e->right;
                    size = 0;
                    while(arg)
                    {
                        size += get_size(DTYPE_INT);
                        arg = arg->left;
                    }
                    if(size)gen_restore_stack(size);
                }
            }
            else if((v = find_global_var(e->text)))
            {
                if(e->right)
                {
                    parse_expr(DTYPE_INT | DTYPE_INT, e->right);
                    gen_push_acc();
                }
                if(e->left)
                {
                    gen_set_acc_global(v->name);
                    parse_index_expr(e, v);
                    gen_load(v->type);
                    cast_dtype(v->type, dt);
                }
                else
                {
                    gen_load_global(v->type, v->name);
                    cast_dtype(v->type, dt);
                }
                if(e->right)
                {
                    gen_call_acc();
                    arg = e->right;
                    size = 0;
                    while(arg)
                    {
                        size += get_size(DTYPE_INT);
                        arg = arg->left;
                    }
                    if(size)gen_restore_stack(size);
                }
            }
            else if((fn = find_function(e->text)))
            {
                if(e->left) error_at(e->line, e->column, "invalid index use.");
                if(e->right)
                {
                    parse_expr(DTYPE_INT | DTYPE_INT, e->right);
                    gen_push_acc();
                }
                gen_call(e->text);
                arg = e->right;
                size = 0;
                while(arg)
                {
                    size += get_size(DTYPE_INT);
                    arg = arg->left;
                }
                if(size)gen_restore_stack(size);
            }
            else error_at(e->line, e->column, "not implemented");
            break;
        case TOK_INTEGER:
            gen_set_acc(e->value);
            break;
        case TOK_ADD:
            if(e->left->tok == TOK_INTEGER)
            {
                parse_expr(dt, e->right);
                gen_add_direct(e->left->value);
            }
            else if(e->right->tok == TOK_INTEGER)
            {
                parse_expr(dt, e->left);
                gen_add_direct(e->right->value);
            }
            else
            {
                parse_expr(dt, e->right);
                gen_push_acc();
                parse_expr(dt, e->left);
                gen_pop_aux();
                gen_add();
            }
            break;
        case TOK_SUB:
            if(e->right->tok == TOK_INTEGER)
            {
                parse_expr(dt, e->left);
                gen_sub_direct(e->right->value);
            }
            else
            {
                parse_expr(dt, e->right);
                gen_push_acc();
                parse_expr(dt, e->left);
                gen_pop_aux();
                gen_sub();
            }
            break;
        case TOK_MUL:
            parse_common_expr(dt, e, 1);
            if(dt & DTYPE_SIGNED)
                gen_smul();
            else 
                gen_umul();
            break;
        case TOK_DIV:
            parse_common_expr(dt, e, 0);
            if(dt & DTYPE_SIGNED)
                gen_sdiv();
            else 
                gen_udiv();
            break;
        case TOK_MOD:
            parse_common_expr(dt, e, 0);
            if(dt & DTYPE_SIGNED)
                gen_smod();
            else 
                gen_umod();
            break;
        case TOK_EQ:
            parse_common_expr(dt, e, 1);
            gen_set_acc_if_eq();
            break;
        case TOK_NE:
            parse_common_expr(dt, e, 1);
            gen_set_acc_if_ne();
            break;
        case TOK_LT:
            parse_common_expr(dt, e, 0);
            if(dt & DTYPE_SIGNED)
                gen_set_acc_if_sl();
            else 
                gen_set_acc_if_ul();
            break;
        case TOK_LE:
            parse_common_expr(dt, e, 0);
            if(dt & DTYPE_SIGNED)
                gen_set_acc_if_sle();
            else 
                gen_set_acc_if_ule();
            break;
        case TOK_GT:
            parse_common_expr(dt, e, 0);
            if(dt & DTYPE_SIGNED)
                gen_set_acc_if_sg();
            else 
                gen_set_acc_if_ug();
            break;
        case TOK_GE:
            parse_common_expr(dt, e, 0);
            if(dt & DTYPE_SIGNED)
                gen_set_acc_if_sge();
            else 
                gen_set_acc_if_uge();
            break;
        case TOK_ATTRIB:
            sub_dt = get_dtype_expr(e->left);
            e->right = optimize(sub_dt, e->right);
            if(e->left->tok == TOK_SYMBOL && find_global_var(e->left->text) && e->left->right == 0 && e->left->left == 0 && e->right->tok == TOK_INTEGER)
            {
                gen_store_global_direct(sub_dt, e->left->text, e->right->value);
            }
            else
            {
                parse_expr(sub_dt, e->right);
                parse_write_expr(sub_dt, e->left);
            }
            break;
        default:
            error_at(e->line, e->column, "invalid read expression.");
            break;
    }
}

void parse_declaration()
{
    dtype_t dt, dt_main;
    char *name;
    expr_t *e, *op;
    dt_main = parse_dtype();
    do
    {
        dt = dt_main;
        while(is_token(TOK_MUL))
        {
            dt = ref_dtype(dt);
            scan();
        }
        name = add_name(get_token()->text);
        e = add_expr_from_token(get_token(), dt);
        match(TOK_SYMBOL, "name");
        if(!is_out_enabled())
            add_local_var(_function, name, dt, 0);
        if(is_token(TOK_ATTRIB))
        {
            op = add_expr_from_token(get_token(), dt);
            scan();
            op->left = e;
            op->right = optimize(dt, expr(dt));
            parse_expr(dt, op);
        }
        if(!is_token(TOK_COMMA)) break;
        scan();
    } while(!is_token(TOK_SEMI));
}

int parse_root_declaration(int is_static)
{
    dtype_t dt, dt_main;
    char *name;
    expr_t *e;
    dt_main = parse_dtype();
    do
    {
        dt = dt_main;
        while(is_token(TOK_MUL))
        {
            dt = ref_dtype(dt);
            scan();
        }
        name = add_name(get_token()->text);
        match(TOK_SYMBOL, "name");
        if(is_token(TOK_PARAMS_OPEN))
        {
            scan();
            match(is_token(TOK_PARAMS_CLOSE), "')'");
            if(is_token(TOK_BLOCK_OPEN))
            {
                _function = find_function(name);
                if(!_function)
                {
                    _function = add_function(!is_static, dt, name);
                }
                gen_function(name, !is_static);
                if(_function->vars_next) gen_reserve_stack(-_function->vars_next);
                parse();
                gen_end_function(name);
                _function = 0;
                return 0;
            }
        }
        else
        {
            if(!is_out_enabled())
                add_global_var(!is_static, name, dt, 0);
            if(is_token(TOK_ATTRIB))
            {
                scan();
                e = optimize(dt, expr(dt));
                if(e->tok == TOK_INTEGER && is_char_dtype(dt))
                {
                    gen_global_char(name, e->value);
                }
                else if(e->tok == TOK_INTEGER && is_integer_dtype(dt))
                {
                    gen_global_int(name, e->value);
                }
                else if(e->tok == TOK_STRING && is_string_dtype(dt))
                {
                    gen_global_string(name, e->text);
                }
                else error("constant expression expected.");
            }
            else
            {
                gen_global_var(name, get_size(dt));
            }
        }
        if(!is_token(TOK_COMMA)) break;
        scan();
    } while(!is_token(TOK_SEMI));
    return 1;
}

void parse_if()
{
    dtype_t dt = DTYPE_INT | DTYPE_SIGNED;
    int lbl_exit = new_label();
    int lbl_else = lbl_exit;
    scan();
    match(is_token(TOK_PARAMS_OPEN), "'('");
    parse_expr(dt, optimize(dt, expr(dt)));
    match(is_token(TOK_PARAMS_CLOSE), "')'");
    gen_jump_if_false(lbl_else);
    parse();
    if(is_token(TOK_KEY_ELSE))
    {
        scan();
        lbl_exit = new_label();
        gen_jump(lbl_exit);
        gen_label(lbl_else);
        parse();
    }
    gen_label(lbl_exit);
}

void parse_switch()
{
    dtype_t dt = DTYPE_INT | DTYPE_SIGNED;
    int old_break_label = _break_label;
    scan();
    match(is_token(TOK_PARAMS_OPEN), "'('");
    parse_expr(dt, optimize(dt, expr(dt)));
    match(is_token(TOK_PARAMS_CLOSE), "')'");
    gen_push_acc();
    _break_label = new_label();
    _case_label = new_label();
    _first_case = is_token(TOK_BLOCK_OPEN) && is_peek(TOK_KEY_CASE);
    parse();
    gen_label(_case_label);
    gen_label(_break_label);
    gen_pop_aux();
    _break_label = old_break_label;
}

void parse_case()
{
    dtype_t dt = DTYPE_INT | DTYPE_SIGNED;
    expr_t *e;
    int next_case_lbl = new_label();
    scan();
    e = optimize(dt, expr(dt));
    if(e->tok != TOK_INTEGER) error("constant expression expected.");
    gen_jump_switch_case(_first_case, _case_label, next_case_lbl, e->value);
    _case_label = next_case_lbl;
    match(is_token(TOK_COLON), "':'");
}

void parse_break()
{
    scan();
    if(_break_label != 0)
    {
        gen_jump(_break_label);
    }
    else
    {
        error("'break' without valid block.");
    }
}

void parse_continue()
{
    scan();
    if(_continue_label != 0)
    {
        gen_jump(_continue_label);
    }
    else
    {
        error("'continue' without valid block.");
    }
}

void parse_while()
{
    dtype_t dt = DTYPE_INT | DTYPE_SIGNED;
    int old_continue_label = _continue_label;
    int old_break_label = _break_label;
    _break_label = new_label();
    _continue_label = new_label();
    gen_label(_continue_label);
    scan();
    match(is_token(TOK_PARAMS_OPEN), "'('");
    parse_expr(dt, optimize(dt, expr(dt)));
    match(is_token(TOK_PARAMS_CLOSE), "')'");
    gen_jump_if_false(_break_label);
    parse();
    gen_jump(_continue_label);
    gen_label(_break_label);
    _continue_label = old_continue_label;
    _break_label = old_break_label;
}

void parse_do()
{
    dtype_t dt = DTYPE_INT | DTYPE_SIGNED;
    int old_continue_label = _continue_label;
    int old_break_label = _break_label;
    _break_label = new_label();
    _continue_label = new_label();
    gen_label(_continue_label);
    scan();
    parse();
    match(is_token(TOK_KEY_WHILE), "'while'");
    match(is_token(TOK_PARAMS_OPEN), "'('");
    parse_expr(dt, optimize(dt, expr(dt)));
    match(is_token(TOK_PARAMS_CLOSE), "')'");
    gen_jump_if_true(_continue_label);
    gen_label(_break_label);
    _continue_label = old_continue_label;
    _break_label = old_break_label;
}

void parse_for()
{
    dtype_t dt = DTYPE_INT | DTYPE_SIGNED;
    int old_continue_label = _continue_label;
    int old_break_label = _break_label;
    expr_t *e_init = 0, *e_cmp = 0, *e_step = 0;
    _break_label = new_label();
    _continue_label = new_label();
    scan();
    match(is_token(TOK_PARAMS_OPEN), "'('");
    if(!is_token(TOK_SEMI)) e_init = expr(dt);
    match(is_token(TOK_SEMI), "';'");
    if(!is_token(TOK_SEMI)) e_cmp = expr(dt);
    match(is_token(TOK_SEMI), "';'");
    if(!is_token(TOK_SEMI)) e_step = expr(dt);
    match(is_token(TOK_PARAMS_CLOSE), "')'");
    if(e_init) parse_expr(DTYPE_INT | DTYPE_SIGNED, e_init);
    gen_label(_continue_label);
    if(e_cmp) parse_expr(DTYPE_INT | DTYPE_SIGNED, e_cmp);
    gen_jump_if_false(_break_label);
    parse();
    if(e_step) parse_expr(DTYPE_INT | DTYPE_SIGNED, e_step);
    gen_jump(_continue_label);
    gen_label(_break_label);
    _continue_label = old_continue_label;
    _break_label = old_break_label;

}

void parse()
{
    int is_static = 0;
    reset_exprs();
    if(is_dtype(get_token()))
    {
        if(_function)
            parse_declaration();
        else if(!parse_root_declaration(is_static)) return;
    }
    else if(is_token(TOK_SYMBOL) && is_peek(TOK_COLON))
    {
        gen_goto_label(get_token()->text);
        scan();
        scan();
        return;
    }
    else if(is_token(TOK_BLOCK_OPEN))
    {
        scan();
        while(!is_token(TOK_EOF) && !is_token(TOK_BLOCK_CLOSE))
        {
            parse();
        }
        match(is_token(TOK_BLOCK_CLOSE), "'}'");
        return;
    }
    else if(_function && is_token(TOK_SYMBOL) || is_token(TOK_MUL))
    {
        if(is_out_enabled())
            parse_expr(DTYPE_VOID, expr(DTYPE_VOID));
        else
            expr(DTYPE_VOID);
    }
    else if(_function && is_token(TOK_KEY_IF))
    {
        parse_if();
        return;
    }
    else if(_function && is_token(TOK_KEY_SWITCH))
    {
        parse_switch();
        return;
    }
    else if(_function && is_token(TOK_KEY_CASE))
    {
        parse_case();
        return;
    }
    else if(_function && is_token(TOK_KEY_WHILE))
    {
        parse_while();
        return;
    }
    else if(_function && is_token(TOK_KEY_DO))
    {
        parse_do();
    }
    else if(_function && is_token(TOK_KEY_FOR))
    {
        parse_for();
        return;
    }
    else if(_function && is_token(TOK_KEY_GOTO) && is_peek(TOK_SYMBOL))
    {
        scan();
        gen_goto(get_token()->text);
        scan();
    }
    else if(_function && is_token(TOK_KEY_BREAK))
    {
        parse_break();
    }
    else if(_function && is_token(TOK_KEY_CONTINUE))
    {
        parse_continue();
    }
    else if(!is_token(TOK_SEMI)) 
    {
        error("unknown command.");
    }
    match(is_token(TOK_SEMI), "';'");
}
