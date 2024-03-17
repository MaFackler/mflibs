#ifndef MF_STRING_H
#define MF_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#ifndef MF_STRING_DEFAULT_SIZE
#define MF_STRING_DEFAULT_SIZE 256
#endif  // MF_STRING_DEFAULT_SIZE

typedef char* MF_String;

typedef struct MF_StringHeader {
    size_t length;
    size_t capacity;
} MF_StringHeader;

// create/delete/attributes
MF_String mf_str_new(void);
MF_String mf_str_news(const char *s);
MF_String mf_str_newn(size_t n);
MF_String mf_str_new_format(const char *fmt, ...);
void mf_str_free(MF_String s);

size_t mf_str_length(MF_String s);
size_t mf_str_capacity(MF_String s);

// manipulation
void mf_str_append(MF_String *s, const char *b);


#ifdef MF_STRING_IMPLEMENTATION


MF_String mf__str_from_header(MF_StringHeader *header) {
    MF_String res = (MF_String) (header + 1);
    return res;
}

size_t mf__str_next_size(size_t orig) {
    int shifts = (int) (log2(orig - 1) + 1);
    return 1 << shifts;
}

MF_StringHeader* mf__str_get_header(MF_String s) {
    return ((MF_StringHeader *) s) -1;
}

MF_StringHeader * mf__str_realloc(MF_StringHeader *header, size_t capacity) {
    MF_StringHeader *res = NULL;
    res = (MF_StringHeader *) realloc(header, sizeof(MF_StringHeader) + capacity);
    res->capacity = capacity;
    return res;
}

MF_StringHeader* mf__str_alloc_by_length(size_t length) {
    size_t capacity = mf__str_next_size(length);
    capacity = (capacity < MF_STRING_DEFAULT_SIZE) ? MF_STRING_DEFAULT_SIZE : capacity;
    assert((capacity & (capacity - 1)) == 0);
    MF_StringHeader *header = (MF_StringHeader *) calloc(1, sizeof(MF_StringHeader) + capacity);
    header->capacity = capacity;
    header->length = 0;
    return header;
}

MF_StringHeader*  mf__str_adjust_size(MF_StringHeader *header, size_t n) {
    MF_StringHeader *res = header;
    if (n != 0) {
        size_t new_capacity = mf__str_next_size(header->length + n + 1);
        if (header->capacity != new_capacity) {
            res = mf__str_realloc(header, new_capacity);
        }
    }

    return res;
}

MF_String mf_str_new(void) {
    MF_StringHeader *header = mf__str_alloc_by_length(0);
    MF_String res = mf__str_from_header(header);
    return res;
}

MF_String mf_str_news(const char *s) {
    size_t len = strlen(s);
    MF_StringHeader *header = mf__str_alloc_by_length(len);
    header->length = len;
    MF_String res = mf__str_from_header(header);
    memcpy(res, s, len);
    res[len] = 0;
    return res;
}

MF_String mf_str_newn(size_t n) {
    MF_StringHeader *header  = mf__str_alloc_by_length(n);
    MF_String res = mf__str_from_header(header);
    return res;
}

MF_String mf_str_new_format(const char *fmt, ...) {
    MF_String res = NULL;
    MF_StringHeader *header = mf__str_alloc_by_length(0);
    res = (char *) (header + 1);

    va_list args;
    va_start(args, fmt);
    vsprintf(res, fmt, args);
    va_end(args);

    return res;
}

void mf_str_free(MF_String s) {
    MF_StringHeader *header = mf__str_get_header(s);
    header->length = 0;
    header->capacity = 0;
    free(header);
}

size_t mf_str_length(MF_String s) {
    MF_StringHeader *header = mf__str_get_header(s);
    return header->length;
}

size_t mf_str_capacity(MF_String s) {
    MF_StringHeader *header = mf__str_get_header(s);
    return header->capacity;
}

void mf_str_append(MF_String *s, const char *b) {
    size_t len = strlen(b);
    MF_StringHeader *header = mf__str_get_header(*s);
    header = mf__str_adjust_size(header, len);
    *s = (MF_String) (header + 1);
    memcpy(&(*s)[header->length], b, len);
    header->length += len;
    (*s)[header->length] = 0;
}

#endif // MF_STRING_IMPLEMENTATION
#endif  // MF_STRING
