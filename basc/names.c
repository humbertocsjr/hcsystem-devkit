#include "basc.h"


static char _names[NAMES_MAX];
static int _names_next = 0;

char *add_name(char *name)
{
    int i;
    char *ptr;
    if(name == 0) return "";
    for(i = 0; i < _names_next; i += strlen(&_names[_names_next]) + 1)
    {
        if(!strcmp(&_names[i], name)) return &_names[i];
    }
    ptr = &_names[_names_next];
    _names_next += strlen(name) + 1;
    if(_names_next >= NAMES_MAX) error("name list overflow at name: %s", name);
    strcpy(ptr, name);
    return ptr;
}
