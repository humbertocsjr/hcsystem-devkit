#ifndef CC_H
#define CC_H

#include "../include/devkit.h"

#define TYPEDEFS_MAX 64
#define NAMES_MAX 10240
#define EXPRS_TEXT_MAX 1024
#define EXPRS_MAX 64
#define TOKEN_MAX 128
#define GLOBALS_MAX 128
#define FUNCTIONS_MAX 96
#define LOCALS_MAX 128

typedef enum tok_t
{
    TOK_EOF,
    TOK_SEMI,
    TOK_INTEGER,
    TOK_SYMBOL,
    TOK_STRING,
    TOK_COMMA,
    TOK_PARAMS_OPEN,
    TOK_PARAMS_CLOSE,
    TOK_INDEX_OPEN,
    TOK_INDEX_CLOSE,
    TOK_BLOCK_OPEN,
    TOK_BLOCK_CLOSE,
    TOK_ADD,
    TOK_SUB,
    TOK_DIV,
    TOK_MUL,
    TOK_EQ,
    TOK_NE,
    TOK_LT,
    TOK_LE,
    TOK_GT,
    TOK_GE,
    TOK_ATTRIB,
    TOK_NOT,
    TOK_AND,
    TOK_AND_ALSO,
    TOK_OR,
    TOK_OR_ELSE,
    TOK_MOD,
    TOK_SHL,
    TOK_SHR,
    TOK_COLON,
    TOK_POINTER,
    TOK_ADDRESSOF,
    TOK_KEY_IF,
    TOK_KEY_ELSE,
    TOK_KEY_WHILE,
    TOK_KEY_DO,
    TOK_KEY_FOR,
    TOK_KEY_CONTINUE,
    TOK_KEY_INT,
    TOK_KEY_CHAR,
    TOK_KEY_UNSIGNED,
    TOK_KEY_SIGNED,
    TOK_KEY_VOID,
    TOK_KEY_SWITCH,
    TOK_KEY_CASE,
    TOK_KEY_BREAK,
    TOK_KEY_GOTO
} tok_t;

enum
{
    DTYPE_UNKNOWN = 0,
    DTYPE_STRUCT_ID_MASK = 0x7f,
    DTYPE_STRUCT = 0x80,
    DTYPE_CHAR = 0x100,
    DTYPE_INT = 0x200,
    DTYPE_LONG = 0x400,
    DTYPE_VOID = 0x800,
    DTYPE_SIGNED = 0x1000,
    DTYPE_UNSIGNED = 0x2000,
    DTYPE_POINTER_MASK = 0xc000,
    DTYPE_POINTER_SHIFT = 14
};
typedef unsigned short dtype_t;

typedef struct token_t
{
    tok_t tok;
    uint16_t line;
    uint16_t column;
    char text[TOKEN_MAX];
} token_t;

typedef struct expr_t
{
    dtype_t type;
    tok_t tok;
    uint16_t line;
    uint16_t column;
    char *text;
    int value;
    struct expr_t *left;
    struct expr_t *right;
} expr_t;

typedef struct typedef_t
{
    char *name;
    dtype_t dt;
} typedef_t;

typedef struct var_t
{
    dtype_t type;
    char *name;
    uint16_t array_size;
    int32_t offset;
    int name_export;
    struct var_t *next;
} var_t;

typedef struct function_t
{
    dtype_t type;
    char *name;
    int name_export;
    int32_t args_next;
    int32_t vars_next;
    var_t *args;
    var_t *vars;
} function_t;

typedef struct source_t
{
    FILE *file;
    char *name;
    uint16_t line;
    uint16_t column;
    int c;
} source_t;

// names.c
char *add_name(char *name);

// error.c
void error(char *fmt, ...);
void error_at(uint16_t line, uint16_t column, char *fmt, ...);
void match(int cmp, char *expected);
void match_at(uint16_t line, uint16_t column, int cmp, char *expected);

// dtype.c
int is_dtype(token_t *t);

// source.c
source_t *get_source();
char get_char();
char read_char();
void process_source(char *file);
token_t *get_token();
token_t *peek_token();
token_t *scan();
int is_token(tok_t tok);
int is_peek(tok_t tok);

// parser.c
void parse();

// out.c
int is_out_enabled();
void open_out(char *file);
void close_out();
void out(char *fmt, ...);

// dtype.c
int is_char_dtype(dtype_t dt);
int is_integer_dtype(dtype_t dt);
int is_string_dtype(dtype_t dt);
dtype_t ref_dtype(dtype_t dt);
dtype_t deref_dtype(dtype_t dt);

// expr.c
expr_t *add_expr(tok_t tok, dtype_t type, uint16_t line, uint16_t column, char *text);
expr_t *add_expr_from_token(token_t *token, dtype_t type);
void reset_exprs();
expr_t *expr(dtype_t type);
expr_t *optimize(dtype_t type, expr_t *e);

// symbols.c
int new_label();
function_t *add_function(int name_export, dtype_t type, char *name);
var_t *add_global_var(int name_export, char *name, dtype_t type, uint16_t array_size);
var_t *add_local_var(function_t *func, char *name, dtype_t type, uint16_t array_size);
var_t *find_global_var(char *name);
function_t *find_function(char *name);
var_t *find_local_var(function_t *func, char *name);

// target.c
int get_size(dtype_t dt);
void cast_dtype(dtype_t src, dtype_t dst);
void gen_header();
void gen_footer();
void gen_global_var(char *name, uint16_t size);
void gen_global_char(char *name, int value);
void gen_global_int(char *name, int value);
void gen_global_string(char *name, char *str);
void gen_set_acc(int value);
void gen_set_aux(int value);
int get_args_offset();
int get_vars_offset();
void gen_store_global(dtype_t dt, char *name);
void gen_store_global_direct(dtype_t dt, char *name, int value);
void gen_function(char *name, int is_public);
void gen_end_function(char *name);
void gen_push_acc();
void gen_push_aux();
void gen_pop_acc();
void gen_pop_aux();
void gen_add();
void gen_add_direct(int value);
void gen_load_global(dtype_t dt, char *name);
void gen_sub();
void gen_sub_direct(int value);
void gen_smul();
void gen_umul();
void gen_sdiv();
void gen_udiv();
void gen_smod();
void gen_umod();
void gen_set_acc_if_eq();
void gen_set_acc_if_ne();
void gen_set_acc_if_sl();
void gen_set_acc_if_ul();
void gen_set_acc_if_sle();
void gen_set_acc_if_ule();
void gen_set_acc_if_sg();
void gen_set_acc_if_ug();
void gen_set_acc_if_sge();
void gen_set_acc_if_uge();
void gen_set_acc_global(char *name);
void gen_set_acc_local(char *name, int offset);
void gen_load(dtype_t dt);
void gen_load_ref_global(dtype_t dt, char *name);
void gen_jump_if_true(int lbl);
void gen_jump_if_false(int lbl);
void gen_label(int lbl);
void gen_jump(int lbl);
void gen_jump_switch_case(int first_case, int lbl_case, int lbl_next_case, int value);
void gen_goto_label(char *name);
void gen_goto(char *name);
void gen_reserve_stack(int size);
void gen_restore_stack(int size);
void gen_load_local(dtype_t dt, char *name, int offset);

#endif
