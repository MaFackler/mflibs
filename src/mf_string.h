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

#define API static inline

typedef char* MF_String;

typedef struct MF_StringHeader {
    size_t length;
    size_t capacity;
} MF_StringHeader;

// create/delete/attributes
API MF_String MF_StringNew(void);
API MF_String MF_StringNewS(const char *s);
API MF_String MF_StringNewN(size_t n);
API MF_String MF_StringNewFormat(const char *fmt, ...);
API void MF_StringFree(MF_String s);

API size_t MF_StringLen(MF_String s);
API size_t MF_StringCapacity(MF_String s);

// manipulation
API void MF_StringAppend(MF_String *s, const char *b);


#ifdef MF_STRING_IMPLEMENTATION


API MF_String MF__StringFromHeader(MF_StringHeader *header) {
    MF_String res = (MF_String) (header + 1);
    return res;
}

API size_t MF__StringNextSize(size_t orig) {
    int shifts = (int) (log2(orig - 1) + 1);
    return 1 << shifts;
}

API MF_StringHeader* MF_StringGetHeader(MF_String s) {
    return ((MF_StringHeader *) s) -1;
}

API MF_StringHeader* MF_StringRealloc(MF_StringHeader *header, size_t capacity) {
    MF_StringHeader *res = NULL;
    res = (MF_StringHeader *) realloc(header, sizeof(MF_StringHeader) + capacity);
    res->capacity = capacity;
    return res;
}

API MF_StringHeader* MF__StringAllocByLength(size_t length) {
    size_t capacity = MF__StringNextSize(length);
    capacity = (capacity < MF_STRING_DEFAULT_SIZE) ? MF_STRING_DEFAULT_SIZE : capacity;
    assert((capacity & (capacity - 1)) == 0);
    MF_StringHeader *header = (MF_StringHeader *) calloc(1, sizeof(MF_StringHeader) + capacity);
    header->capacity = capacity;
    header->length = 0;
    return header;
}

API MF_StringHeader* MF__StringAdjustSize(MF_StringHeader *header, size_t n) {
    MF_StringHeader *res = header;
    if (n != 0) {
        size_t new_capacity = MF__StringNextSize(header->length + n + 1);
        if (header->capacity != new_capacity) {
            res = MF_StringRealloc(header, new_capacity);
        }
    }

    return res;
}

API MF_String MF_StringNew(void) {
    MF_StringHeader *header = MF__StringAllocByLength(0);
    MF_String res = MF__StringFromHeader(header);
    return res;
}

API MF_String MF_StringNewS(const char *s) {
    size_t len = strlen(s);
    MF_StringHeader *header = MF__StringAllocByLength(len);
    header->length = len;
    MF_String res = MF__StringFromHeader(header);
    memcpy(res, s, len);
    res[len] = 0;
    return res;
}

API MF_String MF_StringNewN(size_t n) {
    MF_StringHeader *header  = MF__StringAllocByLength(n);
    MF_String res = MF__StringFromHeader(header);
    return res;
}

API MF_String MF_StringNewFormat(const char *fmt, ...) {
    MF_String res = NULL;
    MF_StringHeader *header = MF__StringAllocByLength(0);
    res = (char *) (header + 1);

    va_list args;
    va_start(args, fmt);
    vsprintf(res, fmt, args);
    va_end(args);

    return res;
}

API void MF_StringFree(MF_String s) {
    MF_StringHeader *header = MF_StringGetHeader(s);
    header->length = 0;
    header->capacity = 0;
    free(header);
}

API size_t MF_StringLen(MF_String s) {
    MF_StringHeader *header = MF_StringGetHeader(s);
    return header->length;
}

API size_t MF_StringCapacity(MF_String s) {
    MF_StringHeader *header = MF_StringGetHeader(s);
    return header->capacity;
}

API void MF_StringAppend(MF_String *s, const char *b) {
    size_t len = strlen(b);
    MF_StringHeader *header = MF_StringGetHeader(*s);
    header = MF__StringAdjustSize(header, len);
    *s = (MF_String) (header + 1);
    memcpy(&(*s)[header->length], b, len);
    header->length += len;
    (*s)[header->length] = 0;
}

#endif // MF_STRING_IMPLEMENTATION
#endif  // MF_STRING
