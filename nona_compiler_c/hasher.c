#include "hasher.h"

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