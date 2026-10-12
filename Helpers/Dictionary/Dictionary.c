//
// Created by Simon Hirsch on 16.09.2026.
//

#include "Dictionary.h"

#include <stdlib.h>
#include <string.h>

static unsigned long hash(const char *s)
{
    unsigned long h = 5381;
    int c;

    while ((c = *s++))
        h = ((h << 5) + h) + c;

    return h;
}

Dict *dict_create(size_t n)
{
    Dict *d = malloc(sizeof(Dict));
    d->nbuckets = n;
    d->buckets = malloc(n * sizeof(Entry *));
    for (size_t i = 0; i < n; i++)
        d->buckets[i] = NULL;

    return d;
}

void dict_set(Dict *d, const char *k, void *v)
{
    size_t idx = hash(k) % d->nbuckets;
    Entry *e = d->buckets[idx];

    while (e) {
        if (strcmp(e->key, k) == 0) {
            e->val = v;
            return;
        }
        e = e->next;
    }

    e = malloc(sizeof(Entry));
    e->key = malloc(strlen(k) + 1);
    strcpy(e->key, k);
    e->val = v;
    e->next = d->buckets[idx];
    d->buckets[idx] = e;
}

void *dict_get(Dict *d, const char *k)
{
    size_t idx = hash(k) % d->nbuckets;
    Entry *e = d->buckets[idx];

    while (e) {
        if (strcmp(e->key, k) == 0)
            return e->val;
        e = e->next;
    }

    return NULL;
}

void dict_free(Dict *d)
{
    for (size_t i = 0; i < d->nbuckets; i++) {
        Entry *e = d->buckets[i];
        while (e) {
            Entry *next = e->next;
            free(e->key);
            free(e);
            e = next;
        }
    }
    free(d->buckets);
    free(d);
}
