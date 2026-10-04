//
// Created by simon on 10.09.2026.
//

#include "symtab.h"

#include "../Helpers/Dictionary/Dictionary.h"
#include <stdlib.h>



Dict *symtab_init()
{
    return dict_create(SYMTAB_MAX_LABELS);
}

void symtab_insert(Dict *d, Label *l)
{
    dict_set(d, l->name, l);
}

Label *symtab_lookup(Dict *d, char *name)
{
    return dict_get(d, name); // NULL, wenn das Label nicht existiert
}