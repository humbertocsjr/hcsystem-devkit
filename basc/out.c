#include "basc.h"

static FILE *_out = 0;
static uint32_t _code_size = 0;
static uint32_t _data_size = 0;

void open_out(char *file)
{
    unlink(file);
    _out = fopen(file, "wb+");
    if(!_out) error("can't create file: %s", file);
}

void close_out()
{
    fclose(_out);
}

void reset_out()
{
    _code_size = 0;
    _data_size = 0;
}

uint32_t get_code_size()
{
    return _code_size;
}

uint32_t get_data_size()
{
    return _data_size;
}

void out_header(aout_t *aout)
{
    fseek(_out, 0, SEEK_SET);
    fwrite(aout, 1, sizeof(aout_t), _out);
    fseek(_out, 0, SEEK_END);
}

uint32_t out_raw(stage_t stage, void *data, size_t size)
{
    uint32_t pos = 0;
    switch(stage)
    {
        case STAGE_CODE_GEN:
            pos = _code_size;
            _code_size+=size;
            break;
        case STAGE_STRING_GEN:
            pos = _data_size;
            _data_size+=size;
            break;
        case STAGE_DATA_GEN:
            pos = _data_size;
            _data_size+=size;
            break;
        case STAGE_SYMBOL_GEN:
            break;
        default:
            error("invalid stage: %d", stage);
            break;
    }
    if(get_stage() != stage) return pos;
    if(_code_size >= 0xfff0) error("code segment overflow.");
    if(_data_size >= 0xfff0) error("data segment overflow.");
    fwrite(data, 1, size, _out);
    return pos;
}

uint32_t out_byte(stage_t stage, uint8_t value)
{
    return out_raw(stage, &value, 1);
}

uint32_t out_word(stage_t stage, uint16_t value)
{
    return out_raw(stage, &value, 2);
}
