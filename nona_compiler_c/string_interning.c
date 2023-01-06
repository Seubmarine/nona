#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "string_interning.h"

#define FNV_OFFSET_BASIS_UINT64 ((uint64_t)14695981039346656037UL)
#define FNV_PRIME_UINT64 ((uint64_t)1099511628211UL)
uint64_t hash_fnv_1a(void *data, size_t byte_len)
{
    uint64_t hash = FNV_OFFSET_BASIS_UINT64;

    for (size_t i = 0; i < byte_len; i++)
    {
        hash = hash ^ ((uint8_t *)data)[i];
        hash = hash * FNV_PRIME_UINT64;
    }
    return (hash);
}

#define NONA_STRING_LEN(str) ((struct nona_string *)(str)->length)
#define DEFAULT_STRING_INTERNER_CAPACITY (16)
#define LOAD_FACTOR (0.75)

struct string_interner string_interner_init(void) {
    struct string_interner si = {.capacity = DEFAULT_STRING_INTERNER_CAPACITY, .data = NULL, .length = 0};
    return (si);
}

void string_interner_free(struct string_interner *si)
{
    if (si->data != NULL)
    {
        for (size_t i = 0; i < si->capacity; i++)
        {
            struct nona_string *str = &si->data[i];
            if (str->string != NULL)
            {
                free(str->string);
                str->string = NULL;
                si->length -= 1;
            }
            if (si->length == 0)
                break;
        }
        free(si->data);
    }
    si->data = NULL;
    si->length = 0;
    si->capacity = 0;
}


struct nona_string *_string_intern_at(struct string_interner *si, char *str, size_t len)
{
    uint64_t hash = hash_fnv_1a(str, len);
    size_t index = hash & (si->capacity - 1);
    size_t index_original = index;
    while (1)
    {
        struct nona_string hm_current = si->data[index];
        if (hm_current.string == NULL) //EMPTY_CELL
            return (&si->data[index]);
        index++;
        if (index >= si->length)
            index = 0;
        if (index == index_original)
            return (NULL);
    }
}


bool string_intern_grow(struct string_interner *si)
{
    if (si->data == NULL)
    {
        si->data = calloc(si->capacity, sizeof(*si->data));
        return (si->data != NULL);
    }
    size_t new_capacity = si->capacity * 2;
    if (new_capacity < si->capacity)
        return (false);
    struct string_interner new_si = string_interner_init();
    new_si.capacity = new_capacity;
    if (string_intern_grow(&new_si) == false)
        return (false);
    for (size_t i = 0; i < si->capacity; i++)
    {
        struct nona_string *current = &si->data[i];
        if (current->string != NULL)
        {
            struct nona_string *new_current = _string_intern_at(&new_si, current->string, current->length);
            *new_current = *current;
            new_si.length += 1;
        }
        else
            return (false); //error finding place in new string interner
    }
    if (new_si.length != si->length)
        return (false); // didn't get the same number of string in the new string interner
    free(si->data);
    *si = new_si;
    return (true);
}

bool nona_string_init(struct nona_string *nona_string, char *str, size_t len)
{
    char *new_str = strndup(str, len);
    if (new_str == NULL)
        return (false);
    nona_string->length = len;    
    nona_string->string = new_str;
    return (true);
}

char *string_intern(struct string_interner *si, char *string, size_t len)
{
    if (si->length / si->capacity >= LOAD_FACTOR || si->length + 1 > si->capacity || si->data == NULL)
        string_intern_grow(si);

    uint64_t hash = hash_fnv_1a(string, len);
    size_t index = hash & (si->capacity - 1);
    size_t index_original = index;
    while (1)
    {
        struct nona_string *si_current = &si->data[index];
        if (si_current->string == NULL) //EMPTY_CELL
        {
            if (!nona_string_init(si_current, string, len)) //create a new nona string and place it inside the string interner
                return (NULL); // failed to create a nona_string
            si->length += 1;
            return (si_current->string);
        }
        else if (si_current->length == len && hash_fnv_1a(si_current->string, si_current->length) == hash && memcmp(si_current->string, string, len) == 0)
        {
            return (si_current->string);
        }
        index++;
        if (index >= si->capacity)
            index = 0;
        if (index == index_original)
            return (NULL);
    }
}