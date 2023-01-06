#ifndef STRING_INTERNING_H
# define STRING_INTERNING_H

struct nona_string {
    char *string;
    size_t length;
};

struct string_interner
{
    size_t capacity;
    size_t length;
    struct nona_string *data;
};

struct string_interner string_interner_init(void);
void string_interner_free(struct string_interner *si);
char *string_intern(struct string_interner *si, char *string, size_t len);
#endif