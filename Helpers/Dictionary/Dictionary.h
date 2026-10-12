//
// Created by Simon Hirsch on 16.09.2026.
//

#ifndef ASSEMBLER_DICTIONARY_H
#define ASSEMBLER_DICTIONARY_H
#include <stddef.h>

typedef struct Entry
{
    char *key;
    void *val;
    struct Entry *next;
} Entry;

typedef struct
{
    Entry **buckets;
    size_t nbuckets;
} Dict;


Dict *dict_create(size_t n);
void dict_set(Dict *d, const char *k, void *v);
void *dict_get(Dict *d, const char *k);
void dict_free(Dict *d);

#endif //ASSEMBLER_DICTIONARY_H
