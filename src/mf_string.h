#ifndef MF_STRING_H
#define MF_STRING_H

#include <mf.h>

#ifndef MF_STRING_DEFAULT_SIZE
#define MF_STRING_DEFAULT_SIZE 256
#endif  // MF_STRING_DEFAULT_SIZE

typedef char* mf_str;

struct mf__str_header {
    u64 size;
    u64 capacity;
};

// create/delete/attributes
mf_str mf_str_new(const char *s);
void mf_str_free(mf_str s);

u64 mf_str_size(mf_str s);
u64 mf_str_capacity(mf_str s);

// manipulation
void mf_str_append(mf_str *s, const char *b);


#ifdef MF_STRING_IMPLEMENTATION

inline mf__str_header* __get_mf_str_header(mf_str s) {
    return ((mf__str_header *) s) -1;
}

inline void __mf_str_realloc(mf_str *s, u64 size) {
    mf__str_header *header = __get_mf_str_header(*s);
    header = (mf__str_header *) realloc(header, sizeof(header) + size);
    header->capacity = size;
    *s = (mf_str) (header + 1);
}

inline void __mf_str_check_and_adjust_size(mf_str *s, i64 n) {
    if (n == 0) {
        return;
    }
    mf__str_header *header = __get_mf_str_header(*s);
    i64 new_capacity = mf_next_power_of_2(header->size + n);
    if (header->capacity != new_capacity) {
        __mf_str_realloc(s, new_capacity);
    }
}

mf_str mf_str_new(const char *s) {
    mf_str res = NULL;   
    mf__str_header *header = (mf__str_header *) malloc(sizeof(mf__str_header) + MF_STRING_DEFAULT_SIZE);
    size_t len = strlen(s);
    header->size = len;
    header->capacity = MF_STRING_DEFAULT_SIZE;
    res = (char *) (header + 1);
    memcpy(res, s, len);
    res[len] = 0;
    return res;
}

void mf_str_free(mf_str s) {
    mf__str_header *header = __get_mf_str_header(s);
    header->size = 0;
    header->capacity = 0;
    free(header);
}


u64 mf_str_size(mf_str s) {
    mf__str_header *header = __get_mf_str_header(s);
    return header->size;
}

u64 mf_str_capacity(mf_str s) {
    mf__str_header *header = __get_mf_str_header(s);
    return header->capacity;
}

void mf_str_append(mf_str *s, const char *b) {
    u64 len = strlen(b);
    mf__str_header *header = __get_mf_str_header(*s);
    __mf_str_check_and_adjust_size(s, len);
    memcpy(&(*s)[header->size], b, len);
    header->size += len;
    (*s)[header->size] = 0;
}

#endif // MF_STRING_IMPLEMENTATION
#endif  // MF_STRING
