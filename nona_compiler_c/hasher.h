#ifndef HASHER_H
# define HASHER_H

# include <inttypes.h>
# include <stdlib.h>
uint64_t hash_fnv_1a(void *data, size_t byte_len);

#endif