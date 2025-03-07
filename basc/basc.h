#ifndef BASC_H
#define BASC_H
#include "../include/devkit.h"

#define NAMES_MAX 10240
#define EXPRS_TEXT_MAX 1024
#define EXPRS_MAX 64
#define TOKEN_MAX 128
#define GLOBALS_MAX 128
#define LEVELS_MAX 16
#define FUNCTIONS_MAX 96
#define LOCALS_MAX 128
#define LABELS_MAX 512

typedef enum stage_t
{
    STAGE_CATALOG,
    STAGE_COMPUTE,
    STAGE_CODE_GEN,
    STAGE_STRING_GEN,
    STAGE_DATA_GEN,
    STAGE_SYMBOL_GEN
} stage_t;

typedef enum tok_t
{
    TOK_EOF,
    TOK_NEW_LINE,
    TOK_INTEGER,
    TOK_SYMBOL,
    TOK_STRING,
    TOK_COMMA,
    TOK_PARAMS_OPEN,
    TOK_PARAMS_CLOSE,
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
    TOK_KEY_NOT,
    TOK_KEY_AND,
    TOK_KEY_AND_ALSO,
    TOK_KEY_OR,
    TOK_KEY_OR_ELSE,
    TOK_KEY_MOD,
    TOK_KEY_SHL,
    TOK_KEY_SHR,
    TOK_KEY_IF,
    TOK_KEY_ELSE,
    TOK_KEY_THEN,
    TOK_KEY_WHILE,
    TOK_KEY_DO,
    TOK_KEY_END,
    TOK_KEY_UNTIL,
    TOK_KEY_FOR,
    TOK_KEY_TO,
    TOK_KEY_STEP,
    TOK_KEY_SUB,
    TOK_KEY_FUNCTION,
    TOK_KEY_DECLARE,
    TOK_KEY_DIM,
    TOK_KEY_AS,
    TOK_KEY_EXIT,
    TOK_KEY_CONTINUE,
    TOK_KEY_INTEGER,
    TOK_KEY_STRING,
    TOK_KEY_PUBLIC,
    TOK_KEY_PRIVATE
} tok_t;

typedef enum type_t
{
    TYPE_NONE = 0,
    TYPE_INTEGER = 0x100,
    TYPE_STRING = 0x200,
    TYPE_SHARED = 0x1000
} type_t;

typedef struct token_t
{
    tok_t tok;
    uint16_t line;
    uint16_t column;
    char text[TOKEN_MAX];
} token_t;

typedef struct expr_t
{
    type_t type;
    tok_t tok;
    uint16_t line;
    uint16_t column;
    char *text;
    int32_t value;
    struct expr_t *left;
    struct expr_t *right;
} expr_t;

typedef struct var_t
{
    type_t type;
    char *name;
    uint16_t array_size;
    int32_t offset;
    int name_export;
    struct var_t *next;
} var_t;

typedef struct function_t
{
    type_t type;
    char *name;
    int name_export;
    int32_t offset;
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

// error.c
void error(char *fmt, ...);
void error_at(uint16_t line, uint16_t column, char *fmt, ...);
void match(int cmp, char *expected);
void match_at(uint16_t line, uint16_t column, int cmp, char *expected);

// names.c
char *add_name(char *name);

// exprs.c
expr_t *add_expr(tok_t tok, type_t type, uint16_t line, uint16_t column, char *text);
expr_t *add_expr_from_token(token_t *token, type_t type);
void reset_exprs();
expr_t *expr(type_t type);
expr_t *optimize(type_t type, expr_t *e);

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

// stage.c
stage_t get_stage();
void set_stage(stage_t stage);

// out.c
void open_out(char *file);
void close_out();
void reset_out();
uint32_t get_code_size();
uint32_t get_data_size();
void out_header(aout_t *aout);
uint32_t out_raw(stage_t stage, void *data, size_t size);
uint32_t out_byte(stage_t stage, uint8_t value);
uint32_t out_word(stage_t stage, uint16_t value);

// parser.c
void set_changed();
int get_changed();
void reset_labels();
void prepare_parser();
void parse();

// symbols.c
function_t *add_function(int name_export, type_t type, char *name, int32_t offset);
var_t *add_global_var(int name_export, char *name, type_t type, uint16_t array_size, int32_t offset);
var_t *add_local_var(function_t *func, char *name, type_t type, uint16_t array_size);
uint16_t generate_symbols();
var_t *find_global_var(char *name);
function_t *find_function(char *name);
var_t *find_local_var(function_t *func, char *name);

// target.c
void gen_global_integer(char *name, int32_t initial_value);
void gen_global_string(char *name, uint16_t size, char *initial_value);
void gen_routine(char *name, type_t type);
void gen_end_routine(char *name, type_t type);
int32_t get_args_offset();
int32_t get_vars_offset();
int32_t get_integer_size();
void gen_reserve_stack(int32_t size);
void gen_release_stack(int32_t size);
void gen_set_acc(int32_t value);
void gen_set_aux(int32_t value);
void gen_store_local(char *name, int32_t offset);
void gen_store_global(char *name, int32_t offset);
void gen_store_local_direct(char *name, int32_t offset, int32_t value);
void gen_store_global_direct(char *name, int32_t offset, int32_t value);
void gen_load_local(char *name, int32_t offset);
void gen_load_global(char *name, int32_t offset);
void gen_push_acc(type_t type);
void gen_push_aux(type_t type);
void gen_pop_acc(type_t type);
void gen_pop_aux(type_t type);
void gen_move_acc_to_aux(type_t type);
void gen_move_aux_to_acc(type_t type);
void gen_add(type_t type);
void gen_add_direct(type_t type, int32_t value);
void gen_sub(type_t type);
void gen_sub_direct(type_t type, int32_t value);
void gen_mul(type_t type);
void gen_div(type_t type);
void gen_mod(type_t type);
void gen_shl(type_t type);
void gen_shr(type_t type);
void gen_set_acc_if_eq(type_t type);
void gen_set_acc_if_ne(type_t type);
void gen_set_acc_if_lt(type_t type);
void gen_set_acc_if_le(type_t type);
void gen_set_acc_if_gt(type_t type);
void gen_set_acc_if_ge(type_t type);
void gen_jump(int32_t address);
void gen_jump_if_eq(type_t type, int32_t address);
void gen_jump_if_ne(type_t type, int32_t address);
void gen_jump_if_lt(type_t type, int32_t address);
void gen_jump_if_le(type_t type, int32_t address);
void gen_jump_if_gt(type_t type, int32_t address);
void gen_jump_if_ge(type_t type, int32_t address);
void gen_jump_if_true(type_t type, int32_t address);
void gen_jump_if_false(type_t type, int32_t address);

#endif
