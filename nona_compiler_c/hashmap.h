#ifndef HASHMAP_H
# define HASHMAP_H
#include <stdbool.h>
#include <stdint.h>

typedef size_t(*hash_function_type)(void *);

typedef _Bool (*key_compare_function)(void *, void *);

typedef void (*key_value_free_function)(void *);

struct hashmap {
    size_t capacity;
    size_t length;
    size_t key_value_size;
    size_t key_offset;
    hash_function_type hash_fn;
    key_compare_function key_compare_fn;
    key_value_free_function key_value_free_fn;
    void *data;
};

struct hashmap hashmap_init(size_t key_value_size, hash_function_type hash_function, key_compare_function key_compare_fn, key_value_free_function key_value_free_fn);
void *hashmap_insert(struct hashmap *hm, void *key_value);
void *hashmap_get(struct hashmap *hm, void *key_value);
void hashmap_free(struct hashmap *hm);

#endif