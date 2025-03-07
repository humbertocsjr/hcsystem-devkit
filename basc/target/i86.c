#include "../basc.h"

void gen_global_integer(char *name, int32_t initial_value)
{
    out_word(STAGE_DATA_GEN, initial_value);
}

void gen_global_string(char *name, uint16_t size, char *initial_value)
{
    uint16_t i;
    char c;
    int include = 1;
    out_word(STAGE_STRING_GEN, size);
    for(i = 0; i < size; i++)
    {
        c = 0;
        if(initial_value && include)
        {
            c = initial_value[i];
            if(c == 0) include = 0;
        }
        out_byte(STAGE_STRING_GEN, c);
    }
}

void gen_routine(char *name, type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x55); // push bp
    out_byte(STAGE_CODE_GEN, 0x89); // mov bp, sp
    out_byte(STAGE_CODE_GEN, 0xe5);
}

void gen_end_routine(char *name, type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x89); // mov sp, bp
    out_byte(STAGE_CODE_GEN, 0xec); 
    out_byte(STAGE_CODE_GEN, 0x5d); // pop bp
    out_byte(STAGE_CODE_GEN, 0xc3); // ret
}

int32_t get_args_offset()
{
    return 4;
}

int32_t get_vars_offset()
{
    return 0;
}

int32_t get_integer_size()
{
    return 2;
}

void gen_reserve_stack(int32_t size)
{
    out_byte(STAGE_CODE_GEN, 0x81); // sub sp, SIZE
    out_byte(STAGE_CODE_GEN, 0xec);
    out_word(STAGE_CODE_GEN, size & 0xffff); 
}

void gen_release_stack(int32_t size)
{
    out_byte(STAGE_CODE_GEN, 0x81); // add sp, SIZE
    out_byte(STAGE_CODE_GEN, 0xc4);
    out_word(STAGE_CODE_GEN, size & 0xffff); 
}

void gen_set_acc(int32_t value)
{
    out_byte(STAGE_CODE_GEN, 0xb8); // mov ax, VALUE
    out_word(STAGE_CODE_GEN, value & 0xffff);
}

void gen_store_local(char *name, int32_t offset)
{
    if(INT8_MIN <= offset && offset <= INT8_MAX)
    {
        out_byte(STAGE_CODE_GEN, 0x89); // mov [bp+OFFSET], ax
        out_byte(STAGE_CODE_GEN, 0x46);
        out_byte(STAGE_CODE_GEN, offset & 0xff);
    }
    else
    {
        out_byte(STAGE_CODE_GEN, 0x89); // mov [bp+OFFSET], ax
        out_byte(STAGE_CODE_GEN, 0x86);
        out_word(STAGE_CODE_GEN, offset & 0xffff);
    }
}

void gen_store_global(char *name, int32_t offset)
{
    out_byte(STAGE_CODE_GEN, 0xa3); // mov [OFFSET], ax
    out_word(STAGE_CODE_GEN, offset & 0xffff);
}

void gen_store_local_direct(char *name, int32_t offset, int32_t value)
{
    if(INT8_MIN <= value && value <= INT8_MAX && INT8_MIN <= offset && offset <= INT8_MAX)
    {
        out_byte(STAGE_CODE_GEN, 0xc6); // mov byte [bp+OFFSET], VALUE
        out_byte(STAGE_CODE_GEN, 0x46);
        out_byte(STAGE_CODE_GEN, offset & 0xffff);
        out_byte(STAGE_CODE_GEN, value & 0xffff);
    }
    else if(INT8_MIN <= value && value <= INT8_MAX)
    {
        out_byte(STAGE_CODE_GEN, 0xc6); // mov byte [bp+OFFSET], VALUE
        out_byte(STAGE_CODE_GEN, 0x86);
        out_word(STAGE_CODE_GEN, offset & 0xffff);
        out_byte(STAGE_CODE_GEN, value & 0xff);
    }
    else
    {
        out_byte(STAGE_CODE_GEN, 0xc7); // mov word [bp+OFFSET], VALUE
        out_byte(STAGE_CODE_GEN, 0x86);
        out_word(STAGE_CODE_GEN, offset & 0xffff);
        out_word(STAGE_CODE_GEN, value & 0xffff);
    }
}

void gen_store_global_direct(char *name, int32_t offset, int32_t value)
{
    if(INT8_MIN <= value && value <= INT8_MAX)
    {
        out_byte(STAGE_CODE_GEN, 0xc6); // mov byte [OFFSET], VALUE
        out_byte(STAGE_CODE_GEN, 0x06);
        out_word(STAGE_CODE_GEN, offset & 0xffff);
        out_byte(STAGE_CODE_GEN, value & 0xff);
    }
    else
    {
        out_byte(STAGE_CODE_GEN, 0xc7); // mov word [OFFSET], VALUE
        out_byte(STAGE_CODE_GEN, 0x06);
        out_word(STAGE_CODE_GEN, offset & 0xffff);
        out_word(STAGE_CODE_GEN, value & 0xffff);
    }
}

void gen_load_local(char *name, int32_t offset)
{
    if(INT8_MIN <= offset && offset <= INT8_MAX)
    {
        out_byte(STAGE_CODE_GEN, 0x8b); // mov ax, [bp+OFFSET]
        out_byte(STAGE_CODE_GEN, 0x46);
        out_byte(STAGE_CODE_GEN, offset & 0xff);
    }
    else
    {
        out_byte(STAGE_CODE_GEN, 0x8b); // mov ax, [bp+OFFSET]
        out_byte(STAGE_CODE_GEN, 0x86);
        out_word(STAGE_CODE_GEN, offset & 0xffff);
    }
}

void gen_load_global(char *name, int32_t offset)
{
    out_byte(STAGE_CODE_GEN, 0xa1); // mov [OFFSET], ax
    out_word(STAGE_CODE_GEN, offset & 0xffff);
}

void gen_push_acc(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x50); // push ax
}

void gen_push_aux(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x53); // push bx
}

void gen_pop_acc(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x58); // pop ax
}

void gen_pop_aux(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x5b); // push bx
}

void gen_move_acc_to_aux(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x89); // mov ax, bx
    out_byte(STAGE_CODE_GEN, 0xc3); 
}

void gen_move_aux_to_acc(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x89); // mov bx, ax
    out_byte(STAGE_CODE_GEN, 0xd8); 
}

void gen_xchg_aux_to_acc(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x93); // xchg ax, bx
    out_byte(STAGE_CODE_GEN, 0x90); 
}

void gen_add(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x01); // add ax, bx
    out_byte(STAGE_CODE_GEN, 0xd8); 
}

void gen_add_direct(type_t type, int32_t value)
{
    if(value == 1)
    {
        out_byte(STAGE_CODE_GEN, 0x40); // inc ax
    }
    else
    {
        out_byte(STAGE_CODE_GEN, 0x05); // add ax, VALUE
        out_word(STAGE_CODE_GEN, value & 0xffff);
    }
}

void gen_sub(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x29); // sub ax, bx
    out_byte(STAGE_CODE_GEN, 0xd8); 
}

void gen_sub_direct(type_t type, int32_t value)
{
    if(value == 1)
    {
        out_byte(STAGE_CODE_GEN, 0x48); // dec ax
    }
    else
    {
        out_byte(STAGE_CODE_GEN, 0x2d); // sub ax, VALUE
        out_word(STAGE_CODE_GEN, value & 0xffff);
    }
}

void gen_mul(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0xf7); // imul bx
    out_byte(STAGE_CODE_GEN, 0xeb);
}

void gen_div(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x31); // xor dx, dx
    out_byte(STAGE_CODE_GEN, 0xd2);
    out_byte(STAGE_CODE_GEN, 0xf7); // idiv bx
    out_byte(STAGE_CODE_GEN, 0xfb);
}

void gen_mod(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x31); // xor dx, dx
    out_byte(STAGE_CODE_GEN, 0xd2);
    out_byte(STAGE_CODE_GEN, 0xf7); // idiv bx
    out_byte(STAGE_CODE_GEN, 0xfb);
    out_byte(STAGE_CODE_GEN, 0x89); // mov ax, dx
    out_byte(STAGE_CODE_GEN, 0xd0);
}

void gen_shl(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x89); // mov cx, ax
    out_byte(STAGE_CODE_GEN, 0xd9);
    out_byte(STAGE_CODE_GEN, 0xd3); // shl ax, cl
    out_byte(STAGE_CODE_GEN, 0xe0);
}

void gen_shr(type_t type)
{
    out_byte(STAGE_CODE_GEN, 0x89); // mov cx, ax
    out_byte(STAGE_CODE_GEN, 0xd9);
    out_byte(STAGE_CODE_GEN, 0xd3); // shl ax, cl
    out_byte(STAGE_CODE_GEN, 0xe8);
}


