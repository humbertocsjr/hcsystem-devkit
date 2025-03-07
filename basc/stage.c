#include "basc.h"

static stage_t _stage = STAGE_CATALOG;

stage_t get_stage()
{
    return _stage;
}

void set_stage(stage_t stage)
{
    _stage = stage;
    reset_out();
    reset_exprs();
}
