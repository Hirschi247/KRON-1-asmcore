//
// Created by Simon Hirsch on 10.09.2026.
//

#ifndef ASSEMBLER_SYMTAB_H
#define ASSEMBLER_SYMTAB_H

#define SYMTAB_MAX_LABELS 128

#include "../Helpers/Dictionary/Dictionary.h"

typedef struct
{
    char *name;
    long address;
} Label;

Dict *symtab_init();

void symtab_insert(Dict *d, Label *l);

Label *symtab_lookup(Dict *d, char *name);

#endif //ASSEMBLER_SYMTAB_H