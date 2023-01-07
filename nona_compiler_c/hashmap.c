#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "hashmap.h"
#include "hasher.h"

#define TOMBSTONE 42

bool is_empty(char *buf, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (buf[i] != 0)
            return (false);
    }
    return (true);
}

size_t  hashmap_get_index(struct hashmap *hm, size_t hash) {
    return ((hash % hm->capacity));
}

#define RETURN_EMPTY        0b000001
#define RETURN_TOMBSTONE    0b000010
#define RETURN_KEY_VALUE    0b000100
#define STOP_AT_EMPTY       0b001000
#define STOP_AT_TOMBSTONE   0b010000
#define STOP_AT_KEY_VALUE   0b100000

bool _is_tombstone(struct hashmap *hm, void *key_value)
{
    if (*(char *)key_value == TOMBSTONE && is_empty(key_value + sizeof(char),  hm->key_value_size - sizeof(char)))
        return (true);
    return (false);
}

void *hashmap_helper_find(struct hashmap *hm, void *key, int action_flag)
{
    size_t hash = hm->hash_fn(key);
    size_t index = hashmap_get_index(hm, hash);
    size_t index_origin = index;
    while (1)
    {
        void *hm_key_value = hm->data + (index * hm->key_value_size);
        if (is_empty(hm_key_value, hm->key_value_size))
        {
            if (action_flag & STOP_AT_EMPTY)
            {
                if (action_flag & RETURN_EMPTY)
                    return (hm_key_value);
                return (NULL);
            }
        }
        else if (_is_tombstone(hm, hm_key_value))
        {
            if (action_flag & STOP_AT_TOMBSTONE)
            {
                if (action_flag & RETURN_TOMBSTONE)
                    return (hm_key_value);
                return (NULL);
            }
        }
        else if ((action_flag & STOP_AT_KEY_VALUE) && hm->hash_fn(hm_key_value) == hash && hm->key_compare_fn(key, hm_key_value))
        {
            if (action_flag & RETURN_KEY_VALUE)
                return (hm_key_value);
            return (NULL);
        }
        index++;
        if (index >= hm->capacity)
            index = 0;
        if (index == index_origin)
            return (NULL);
    }
}

bool hashmap_destroy(struct hashmap *hm, void *key)
{
    void *hm_key_value = hashmap_helper_find(hm, key, STOP_AT_EMPTY | STOP_AT_KEY_VALUE | RETURN_KEY_VALUE);
    if (hm_key_value == NULL)
        return (false);
    hm->key_value_free_fn(hm_key_value);
    bzero(hm_key_value, hm->key_value_size);
    *(char *)hm_key_value = TOMBSTONE;
    hm->length -= 1;
    return (true);
}

struct hashmap hashmap_init(size_t key_value_size, hash_function_type hash_function, key_compare_function key_compare_fn, key_value_free_function key_value_free_fn) {
    struct hashmap hm;

    hm.capacity = 8;
    hm.length = 0;
    hm.key_value_size = key_value_size;
    hm.hash_fn = hash_function;
    hm.key_compare_fn = key_compare_fn;
    hm.key_value_free_fn = key_value_free_fn;
    hm.data = malloc(hm.key_value_size * hm.capacity);
    bzero(hm.data, hm.key_value_size * hm.capacity);
    return (hm);
}

/*
Check if a buffer is empty with only 0
https://stackoverflow.com/questions/1493936/faster-approach-to-checking-for-an-all-zero-buffer-in-c
*/

#include <stdbool.h>
void *hashmap_get(struct hashmap *hm, void *key_value)
{
    void *hm_key_value = hashmap_helper_find(hm, key_value, STOP_AT_EMPTY | STOP_AT_TOMBSTONE | STOP_AT_KEY_VALUE | RETURN_KEY_VALUE);
    return (hm_key_value);
}

void *hashmap_insert(struct hashmap *hm, void *key_value)
{
    size_t hash = hm->hash_fn(key_value);
    size_t index = hashmap_get_index(hm, hash);
    while (1)
    {
        void *hm_key_value = hm->data + (index * hm->key_value_size);
        if (is_empty(hm_key_value, hm->key_value_size))
        {
            memcpy(hm_key_value, key_value, hm->key_value_size);
            hm->length += 1;
            return (hm_key_value);
        }
        else if (hm->hash_fn(hm_key_value) == hash && hm->key_compare_fn(hm_key_value, key_value))
        {
            memcpy(hm_key_value, key_value, hm->key_value_size);
            hm->length += 1;
            return (hm_key_value);
        }
        index++;
        if (index > hm->capacity)
            index = 0;
    }
}

// #include <stdio.h>
// #include <string.h>
// #include <inttypes.h>

// struct nona_identifier
// {
//     char *identifier;
//     int v;
// };

// int main(int argc, char const *argv[])
// {
//     struct nona_identifier arr[] = {{"add_one"}, {"n"}, {"int"}, {"x"}, {"int"}, {"add_one"}, {"x"}};
    
//     struct hashmap string_container = hashmap_init(sizeof(struct nona_identifier), hash_fnv_1a_nona_identifier, nona_identifier_compare, nona_identifier_free);
//     for (size_t i = 0; i < sizeof(arr) / sizeof(*arr); i++)
//     {
//         struct nona_identifier *get_result = hashmap_get(&string_container, &arr[i]);
//         if (get_result == NULL)
//         {
//             struct nona_identifier copy = {.identifier = strdup(arr[i].identifier)};
//             struct nona_identifier *tmp = hashmap_insert(&string_container, &copy);
//             arr[i] = *tmp;
//         }
//         else
//             arr[i] = *get_result;
//     }
//     hashmap_insert(&string_container, &(struct nona_identifier){"x", 123456789});
//     printf("%p == %p\n", arr[2].identifier, arr[4].identifier);
//     hashmap_destroy(&string_container, &(struct nona_identifier){"add_one"});
//     hashmap_destroy(&string_container, &(struct nona_identifier){"x"});
//     hashmap_destroy(&string_container, &(struct nona_identifier){"int"});
//     hashmap_destroy(&string_container, &(struct nona_identifier){"int"});
//     hashmap_destroy(&string_container, &(struct nona_identifier){"n"});
//     free(string_container.data);
//     return 0;
// }