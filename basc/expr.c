#include "basc.h"

static char _exprs_text[EXPRS_TEXT_MAX];
static int _exprs_text_next = 0;
static expr_t _exprs[EXPRS_TEXT_MAX];
static int _exprs_next = 0;

expr_t *add_expr(tok_t tok, type_t type, uint16_t line, uint16_t column, char *text)
{
    expr_t *e = &_exprs[_exprs_next];
    _exprs_next++;
    if(_exprs_next > EXPRS_TEXT_MAX)
    {
        error_at(line, column, "expression table overflow.");
    }
    e->tok = tok;
    e->type = type;
    e->line = line;
    e->column = column;
    e->value = 0;
    e->left = 0;
    e->right = 0;
    if(strlen(text))
    {
        e->text = &_exprs_text[_exprs_text_next];
        _exprs_text_next += strlen(text) + 1;
        if(_exprs_text_next >= EXPRS_TEXT_MAX)
        {
            error_at(line, column, "extression text list overflow.");
        }
        strcpy(e->text, text);
        if(e->tok == TOK_INTEGER) e->value = atoi(text);
    }
    else
    {
        e->text = "";
    }
    return e;
}

expr_t *add_expr_from_token(token_t *token, type_t type)
{
    expr_t *e = add_expr(token->tok, type, token->line, token->column, token->text);
    return e;
}

void reset_exprs()
{
    _exprs_next = 0;
    _exprs_text_next = 0;
}

expr_t *expr_value(type_t type)
{
    expr_t *e = 0;
    if(is_token(TOK_INTEGER) || is_token(TOK_STRING))
    {
        e = add_expr_from_token(get_token(), type);
        scan();
    }
    else if(is_token(TOK_PARAMS_OPEN))
    {
        scan();
        e = expr(type);
        match(TOK_PARAMS_CLOSE, "')'");
    }
    else if(is_token(TOK_SYMBOL) && is_peek(TOK_PARAMS_OPEN))
    {
        e = add_expr_from_token(get_token(), type);
        scan();
        e->right = expr(TYPE_INTEGER);
    }
    else if(is_token(TOK_SYMBOL))
    {
        e = add_expr_from_token(get_token(), type);
        scan();
    }
    else error("expression expected.");
    return e;
}

expr_t *expr_shift(type_t type)
{
    expr_t *e = expr_value(type);
    expr_t *op;
    while(is_token(TOK_KEY_SHL) || is_token(TOK_KEY_SHR))
    {
        op = add_expr_from_token(get_token(), type);
        scan();
        op->left = e;
        op->right = expr_value(type);
        e = op;
    }
    return e;
}

expr_t *expr_mul(type_t type)
{
    expr_t *e = expr_shift(type);
    expr_t *op;
    while(is_token(TOK_KEY_MOD) || is_token(TOK_MUL) || is_token(TOK_DIV))
    {
        op = add_expr_from_token(get_token(), type);
        scan();
        op->left = e;
        op->right = expr_shift(type);
        e = op;
    }
    return e;
}

expr_t *expr_add(type_t type)
{
    expr_t *e = expr_mul(type);
    expr_t *op;
    while(is_token(TOK_ADD) || is_token(TOK_SUB))
    {
        op = add_expr_from_token(get_token(), type);
        scan();
        op->left = e;
        op->right = expr_mul(type);
        e = op;
    }
    return e;
}

expr_t *expr_and(type_t type)
{
    expr_t *e = expr_add(type);
    expr_t *op;
    while(is_token(TOK_KEY_AND) || is_token(TOK_KEY_AND_ALSO))
    {
        op = add_expr_from_token(get_token(), type);
        scan();
        op->left = e;
        op->right = expr_add(type);
        e = op;
    }
    return e;
}

expr_t *expr_or(type_t type)
{
    expr_t *e = expr_and(type);
    expr_t *op;
    while(is_token(TOK_KEY_OR) || is_token(TOK_KEY_OR_ELSE))
    {
        op = add_expr_from_token(get_token(), type);
        scan();
        op->left = e;
        op->right = expr_and(type);
        e = op;
    }
    return e;
}

expr_t *expr_cmp(type_t type)
{
    expr_t *e = expr_or(type);
    expr_t *op;
    while(is_token(TOK_NE) || is_token(TOK_LT) || is_token(TOK_LE) || is_token(TOK_GT) || is_token(TOK_GE))
    {
        op = add_expr_from_token(get_token(), type);
        scan();
        op->left = e;
        op->right = expr_or(type);
        e = op;
    }
    return e;
}

expr_t *expr_eq(type_t type)
{
    expr_t *e = expr_cmp(type);
    expr_t *op;
    while(is_token(TOK_EQ))
    {
        op = add_expr_from_token(get_token(), type);
        scan();
        op->left = e;
        op->right = expr_cmp(type);
        e = op;
    }
    return e;
}

expr_t *expr(type_t type)
{
    return expr_eq(type);
}

expr_t *optimize(type_t type, expr_t *e)
{
    if(e == 0) return e;
    if(e->left) e->left = optimize(type, e->left);
    if(e->right) e->right = optimize(type, e->right);
    if(e->left == 0 || e->right == 0) return e;
    if(e->left->tok == e->tok && e->left->right->tok == TOK_INTEGER && e->right->tok == TOK_INTEGER)
    {
        switch(e->tok)
        {
            case TOK_ADD:
                e->right->value = e->left->right->value + e->right->value;
                e->left = e->left->left;
                break;
            default:
                return e;
        }
    }
    if(e->left->tok == TOK_INTEGER && e->right->tok == TOK_INTEGER)
    {
        switch(e->tok)
        {
            case TOK_ADD:
                e->tok = TOK_INTEGER;
                e->value = e->left->value + e->right->value;
                break;
            case TOK_SUB:
                e->tok = TOK_INTEGER;
                e->value = e->left->value - e->right->value;
                break;
            case TOK_MUL:
                e->tok = TOK_INTEGER;
                e->value = e->left->value * e->right->value;
                break;
            case TOK_DIV:
                e->tok = TOK_INTEGER;
                e->value = e->left->value / e->right->value;
                break;
            case TOK_KEY_MOD:
                e->tok = TOK_INTEGER;
                e->value = e->left->value % e->right->value;
                break;
            case TOK_KEY_SHL:
                e->tok = TOK_INTEGER;
                e->value = e->left->value << e->right->value;
                break;
            case TOK_KEY_SHR:
                e->tok = TOK_INTEGER;
                e->value = e->left->value >> e->right->value;
                break;
            case TOK_EQ:
                e->tok = TOK_INTEGER;
                e->value = e->left->value == e->right->value ? 1 : 0;
                break;
            case TOK_NE:
                e->tok = TOK_INTEGER;
                e->value = e->left->value != e->right->value ? 1 : 0;
                break;
            case TOK_LT:
                e->tok = TOK_INTEGER;
                e->value = e->left->value < e->right->value ? 1 : 0;
                break;
            case TOK_LE:
                e->tok = TOK_INTEGER;
                e->value = e->left->value <= e->right->value ? 1 : 0;
                break;
            case TOK_GT:
                e->tok = TOK_INTEGER;
                e->value = e->left->value > e->right->value ? 1 : 0;
                break;
            case TOK_GE:
                e->tok = TOK_INTEGER;
                e->value = e->left->value >= e->right->value ? 1 : 0;
                break;
            default:
                return e;
        }
    }
    return e;
}
