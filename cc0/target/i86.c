#include "../cc.h"

int get_size(dtype_t dt)
{
    if(dt & DTYPE_CHAR) return 1;
    if(dt & DTYPE_INT) return 2;
    if(dt & DTYPE_POINTER_MASK) return 2;
    error("unsupported data type size. 0x%x", dt);
    return 0;
}

void cast_dtype(dtype_t src, dtype_t dst)
{
    if(src == dst) return;
    if(((src & DTYPE_POINTER_MASK) != 0) == ((dst & DTYPE_POINTER_MASK) != 0)) return;
    if(((src & DTYPE_POINTER_MASK) != 0) == ((dst & DTYPE_INT) != 0)) return;
    if(((src & DTYPE_INT) != 0) == ((dst & DTYPE_POINTER_MASK) != 0)) return;
    if(((src & DTYPE_CHAR) != 0) == ((dst & DTYPE_INT) != 0))
    {
        if(((src & DTYPE_SIGNED) != 0) == ((dst & DTYPE_SIGNED) != 0))
            out("cbw");
        else
            out("xor ah, ah");
    }
}

void gen_header()
{
    out("section .text");
}

void gen_footer()
{
}

void gen_global_var(char *name, uint16_t size)
{
    out("section .bss");
    out("_%s:", name);
    out("resb %u", (unsigned)size);
    out("section .text");
}

void gen_global_char(char *name, int value)
{
    out("section .data");
    out("_%s:", name);
    out("db %d", value);
    out("section .text");
}

void gen_global_int(char *name, int value)
{
    out("section .data");
    out("_%s:", name);
    out("dw %d", value);
    out("section .text");
}

void gen_global_string(char *name, char *str)
{
    out("section .data");
    out("_%s:", name);
    while(*str) 
    {
        out("db %d ; '%c'", *str, *str >= 32 && *str < 127 ? *str : ' ');
        str++;
    }
    out("db 0");
    out("section .text");
}

void gen_set_acc(int value)
{
    out("mov ax, %d", value);
}

void gen_set_aux(int value)
{
    out("mov bx, %d", value);
}

int get_args_offset()
{
    return 4;
}

int get_vars_offset()
{
    return 0;
}

void gen_store_global(dtype_t dt, char *name)
{
    if(is_char_dtype(dt))
    {
        out("mov [_%s], al", name);
    }
    else if(is_integer_dtype(dt))
    {
        out("mov [_%s], ax", name);
    }
    else error("invalid data type.");
}

void gen_store_global_direct(dtype_t dt, char *name, int value)
{
    if(is_char_dtype(dt))
    {
        out("mov byte [_%s], %d", name, value);
    }
    else if(is_integer_dtype(dt))
    {
        out("mov word [_%s], %d", name, value);
    }
    else error("invalid data type.");
}

void gen_function(char *name, int is_public)
{
    if(is_public)out("global _%s", name);
    out("_%s:", name);
    out("push bp");
    out("mov bp, sp");
}

void gen_end_function(char *name)
{
    out("mov sp, bp");
    out("pop bp");
    out("ret");
}

void gen_push_acc()
{
    out("push ax");
}

void gen_push_aux()
{
    out("push bx");
}

void gen_pop_acc()
{
    out("pop ax");
}

void gen_pop_aux()
{
    out("pop bx");
}

void gen_add()
{
    out("add ax, bx");
}

void gen_add_direct(int value)
{
    out("add ax, %d", value);
}

void gen_load_global(dtype_t dt, char *name)
{
    if(is_char_dtype(dt))
    {
        out("mov al, [_%s]", name);
        if(dt & DTYPE_SIGNED)
        {
            out("cbw");
        }
        else
        {
            out("xor ah, ah");
        }
    }
    else if(is_integer_dtype(dt))
    {
        out("mov ax, [_%s]", name);
    }
    else error("invalid data type.");
}

void gen_sub()
{
    out("sub ax, bx");
}

void gen_sub_direct(int value)
{
    out("sub ax, %d", value);
}

void gen_smul()
{
    out("imul bx");
}

void gen_umul()
{
    out("mul bx");
}

void gen_sdiv()
{
    out("xor dx dx");
    out("idiv bx");
}

void gen_udiv()
{
    out("xor dx dx");
    out("div bx");
}

void gen_smod()
{
    out("xor dx dx");
    out("idiv bx");
    out("mov ax dx");
}

void gen_umod()
{
    out("xor dx dx");
    out("div bx");
    out("mov ax dx");
}

void gen_set_acc_if_eq()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("je L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_ne()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("jne L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_ul()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("jb L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_sl()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("jl L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_ule()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("jbe L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_sle()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("jle L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_ug()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("ja L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_sg()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("jg L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_uge()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("jae L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_if_sge()
{
    int lbl = new_label();
    out("cmp ax, bx");
    out("mov ax, 1");
    out("jge L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
}

void gen_set_acc_global(char *name)
{
    out("mov ax, _%s", name);
}

void gen_load(dtype_t dt)
{
    out("mov si, ax");
    if(is_char_dtype(dt))
    {
        out("mov al, [si]");
    }
    else if(is_integer_dtype(dt))
    {
        out("mov ax, [si]");
    }
    else error("invalid data type.");
}

void gen_load_ref_global(dtype_t dt, char *name)
{
    out("mov si, [_%s]", name);
    if(is_char_dtype(dt))
    {
        out("mov al, [si]");
    }
    else if(is_integer_dtype(dt))
    {
        out("mov ax, [si]");
    }
    else error("invalid data type.");
}

void gen_jump_if_true(int lbl)
{
    int lbl_else = new_label();
    out("or ax, ax");
    out("jne L%d", lbl_else);
    out("jmp L%d", lbl);
    out("L%d:", lbl_else);
}

void gen_jump_if_false(int lbl)
{
    int lbl_else = new_label();
    out("or ax, ax");
    out("je L%d", lbl_else);
    out("jmp L%d", lbl);
    out("L%d:", lbl_else);
}

void gen_label(int lbl)
{
    out("L%d:", lbl);
}

void gen_jump(int lbl)
{
    out("jmp L%d", lbl);
}

void gen_jump_switch_case(int first_case, int lbl_case, int lbl_next_case, int value)
{
    int lbl_true = new_label();
    out("L%d:", lbl_case);
    if(!first_case)
    {
        out("pop ax");
        out("push ax");
    }
    out("cmp ax, %d", value);
    out("je L%d", lbl_true);
    out("jmp L%d", lbl_next_case);
    out("L%d:", lbl_true);
}

void gen_goto_label(char *name)
{
    out("%s:", name);
}

void gen_goto(char *name)
{
    out("jmp %s", name);
}

void gen_reserve_stack(int size)
{
    out("sub sp, %d", size);
}

void gen_restore_stack(int size)
{
    out("add sp, %d", size);
}

void gen_load_local(dtype_t dt, char *name, int offset)
{
    if(is_char_dtype(dt))
    {
        out("mov al, [bp+%d] ; %s", offset, name);
    }
    else if(is_integer_dtype(dt))
    {
        out("mov ax, [bp+%d] ; %s", offset, name);
    }
    else error("invalid data type.");
}
