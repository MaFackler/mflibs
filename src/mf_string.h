#ifndef MF_STRING_H
#define MF_STRING_H

#include <mf.h>

#ifndef MF_STRING_DEFAULT_SIZE
#define MF_STRING_DEFAULT_SIZE 256
#endif  // MF_STRING_DEFAULT_SIZE


#define S(a) mf_strview { a, strlen(a) }

struct mf_strview {
    const char *data;
    u64 size;
};

typedef char* mf_str;

struct mf__str_header {
    u64 size;
    u64 capacity;
};

// create/delete/attributes
mf_str mf_str_new();
mf_str mf_str_new(const char *s);
mf_str mf_str_new(size_t n);
mf_str mf_str_new_format(const char *fmt, ...);
void mf_str_free(mf_str s);

u64 mf_str_size(mf_str s);
u64 mf_str_capacity(mf_str s);

// manipulation
void mf_str_append(mf_str *s, const char *b);


#ifdef MF_STRING_IMPLEMENTATION

inline mf_str mf__str_from_header(mf__str_header *header) {
    mf_str res = (mf_str) (header + 1);
    return res;
}

inline mf__str_header* mf__str_get_header(mf_str s) {
    return ((mf__str_header *) s) -1;
}

inline mf__str_header * mf__str_realloc(mf__str_header *header, u64 size) {
    mf__str_header *res = NULL;
    res = (mf__str_header *) realloc(header, sizeof(header) + size);
    res->capacity = size;
    return header;
}

inline mf__str_header* mf__str_alloc_size(u64 size) {
    size = mf_next_power_of_2(size);
    size = MF_Max(size, MF_STRING_DEFAULT_SIZE);
    MF_Assert((size & (size - 1)) == 0);
    mf__str_header *header = (mf__str_header *) calloc(1, sizeof(mf__str_header) + size);
    header->capacity = size;
    header->size = 0;
    return header;
}

inline mf__str_header*  __mf_str_check_and_adjust_size(mf__str_header *header, i64 n) {
    mf__str_header *res = header;
    if (n != 0) {
        i64 new_capacity = mf_next_power_of_2(header->size + n);
        if (header->capacity != new_capacity) {
            res = mf__str_realloc(header, new_capacity);
        }
    }
    return res;
}

mf_str mf_str_new() {
    mf__str_header *header = mf__str_alloc_size(0);
    mf_str res = mf__str_from_header(header);
    return res;
}

mf_str mf_str_new(const char *s) {
    size_t len = strlen(s);
    mf__str_header *header = mf__str_alloc_size(len);
    header->size = len;
    mf_str res = mf__str_from_header(header);
    memcpy(res, s, len);
    res[len] = 0;
    return res;
}

mf_str mf_str_new(size_t n) {
    mf__str_header *header  = mf__str_alloc_size(n);
    mf_str res = mf__str_from_header(header);
    return res;
}

mf_str mf_str_new_format(const char *fmt, ...) {
    mf_str res = NULL;
    mf__str_header *header = mf__str_alloc_size(0);
    res = (char *) (header + 1);

    va_list args;
    va_start(args, fmt);
    vsprintf(res, fmt, args);
    va_end(args);

    return res;
}

void mf_str_free(mf_str s) {
    mf__str_header *header = mf__str_get_header(s);
    header->size = 0;
    header->capacity = 0;
    free(header);
}


u64 mf_str_size(mf_str s) {
    mf__str_header *header = mf__str_get_header(s);
    return header->size;
}

u64 mf_str_capacity(mf_str s) {
    mf__str_header *header = mf__str_get_header(s);
    return header->capacity;
}

void mf_str_append(mf_str *s, const char *b) {
    u64 len = strlen(b);
    mf__str_header *header = mf__str_get_header(*s);
    header = __mf_str_check_and_adjust_size(header, len);
    memcpy(&(*s)[header->size], b, len);
    header->size += len;
    (*s)[header->size] = 0;
}

#endif // MF_STRING_IMPLEMENTATION
#endif  // MF_STRING
