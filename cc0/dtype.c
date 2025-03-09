#include "cc.h"

int is_char_dtype(dtype_t dt)
{
    if(dt & DTYPE_CHAR) return 1;
    return 0;
}

int is_integer_dtype(dtype_t dt)
{
    if(dt & DTYPE_POINTER_MASK) return 1;
    if(dt & DTYPE_INT) return 1;
    if(dt & DTYPE_CHAR) return 1;
    return 0;
}

int is_string_dtype(dtype_t dt)
{
    if((dt & DTYPE_POINTER_MASK) && (dt & DTYPE_CHAR)) return 1;
    return 0;
}

dtype_t ref_dtype(dtype_t dt)
{
    dtype_t pointer = (dt & DTYPE_POINTER_MASK) >> DTYPE_POINTER_SHIFT;
    if((dt & DTYPE_POINTER_MASK) == DTYPE_POINTER_MASK)
    {
        error("impossible data type reference.");
    }
    pointer++;
    dt &= ~DTYPE_POINTER_MASK;
    dt |= pointer << DTYPE_POINTER_SHIFT;
    return dt;
}

dtype_t deref_dtype(dtype_t dt)
{
    dtype_t pointer = (dt & DTYPE_POINTER_MASK) >> DTYPE_POINTER_SHIFT;
    if((dt & DTYPE_POINTER_MASK) == 0)
    {
        error("impossible data type de-reference.");
    }
    pointer--;
    dt &= ~DTYPE_POINTER_MASK;
    dt |= pointer << DTYPE_POINTER_SHIFT;
    return dt;
}
