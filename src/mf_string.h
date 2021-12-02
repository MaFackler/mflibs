#ifndef MF_STRING_H
#define MF_STRING_H
#include <ctype.h>

// Allocators
#ifndef mf_str_malloc
    #define mf_str_malloc malloc
#endif
#ifndef mf_str_memfree
    #define mf_str_memfree free
#endif
#ifndef mf_str_realloc
    #define mf_str_realloc
#endif

void mf_str_free(char *a);
char* mf_str_new(const char* fmt, ...);
bool mf_str_endswith(const char* a, const char* b);
bool mf_str_is_substrn(const char* a, const char* b, size_t n);
bool mf_str_is_substr(const char* a, const char* b);
bool mf_str_is_equal(const char* a, const char* b);
bool mf_str_is_empty(const char* a);
bool mf_str_is_fuzzy(const char* a, const char* b);
void mf_str_append(char* *a, const char* b);
void mf_str_nappend(char* a, int n, ...);
char* mf_str_subdup(char* a, size_t start, size_t end);
u32 mf_str_count_char(const char* a, char c);
u32 mf_str_count_char_break(const char* a, char c);
size_t mf_strlen(const char* a);

char* mf_strdup(const char* a);
void mf_strcpy(char* dest, const char* source);

// char buffer utilities
void mf_to_cbuf(char *buf, u32 a);
void mf_cbuf_rfill(char *buf, size_t len, char c, size_t n);

#define mf_strcat strcat
#define mf_strncat strncat

#ifdef MF_STRING_IMPLEMENTATION

// String utilities

char* mf_str_new(size_t n) {
    char* res = (char*) mf_str_malloc(n);
    return res;
}

void mf_str_free(char* a) {
    if (a)
        mf_str_memfree(a);
}

char* mf_str_new(const char* fmt, ...) {
    va_list args;
    // TODO: safety???
    char buffer[1024] = {};
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    char* res = mf_strdup(&buffer[0]);
    return res;
}

bool mf_str_endswith(const char* a, const char* b) {
    size_t aSize = strlen(a);
    size_t bSize = strlen(b);

    bool res = strncmp(a + aSize - bSize, b, bSize) == 0;
    return res;
}

bool mf_str_is_substrn(const char* a, const char* b, size_t n) {
    bool res = false;

    size_t sizeA = strlen(a);

    if (n <= sizeA) {
        for (size_t i = 0; i <= sizeA - n; ++i) {
            size_t j;
            for (j = 0; j < n; ++j) {
                if (a[i + j] != b[j])
                    break;

            }
            if (j == n) {
                res = true;
                break;
            }

        }
    }
    return res;
}

bool mf_str_is_substr(const char* a, const char* b) {
    bool res = mf_str_is_substrn(a, b, mf_strlen(b));
    return res;
}

bool mf_str_is_empty(const char* a) {
    if (a == NULL)
        return true;
    return mf_strlen(a) == 0;
}

bool mf_str_is_fuzzy(const char* a, const char* b) {
    size_t indexB = 0;
    bool caseSensitive = false;
    size_t sizeA = mf_strlen(a);
    size_t sizeB = mf_strlen(b);
    if (sizeB == 0)
        return true;
    for (size_t indexA = 0; indexA < sizeA; ++indexA) {
        char ca = a[indexA];
        char cb = b[indexB];

        caseSensitive = caseSensitive || isupper(cb);
        if (!caseSensitive) {
            ca = tolower(ca);
            cb = tolower(cb);
        }
        if (ca == cb) {
            indexB++;
            if (indexB == sizeB)
                break;
        }
    }
    return indexB == sizeB;
}

void mf_str_append(char** a, const char* b) {
    u32 sizeA = mf_strlen(*a);
    u32 sizeB = mf_strlen(b);
    char* neww = (char*) mf_str_realloc(*a, sizeA + sizeB + 1);
    for (size_t i = 0; i < mf_strlen(b); ++i) {
        neww[sizeA + i] = b[i];
    }
    neww[sizeA + sizeB] = 0;
    *a = neww;
}

void mf_str_nappend(char* a, int n, ...) {
    va_list ap;

    va_start(ap, n);
    for (size_t i = 0; i < n; ++i) {
        const char* value = va_arg(ap, const char*);
        mf_str_append(&a, value);
    }
    va_end(ap);
}

char* mf_str_subdup(char* a, size_t start, size_t end) {
    size_t n = end - start;
    char* res = mf_str_new(n + 1);
    for (size_t i = 0; i < n; ++i) {
        res[i] = a[start + i];
    }
    res[n + 1] = 0;
    return res;
}

bool mf_str_is_equal(const char* a, const char* b) {
    bool res = strcmp(a, b) == 0;
    return res;
}

u32 mf_str_count_char(const char* a, char c) {
    u32 res = 0;
    while(*a) {
        if (*a++ == c)
            res++;
    }
    return res;
}

u32 mf_str_count_char_break(const char* a, char c) {
    u32 res = 0;
    while (*a) {
        if (*a++ ==c)
            res++;
        else
            break;
    }
    return res;
}

size_t mf_strlen(const char* a) {
    size_t res = 0;
    if (a != NULL)
        res = strlen(a);
    return res;
}

void mf_to_cbuf(char *buf, u32 a) {
    sprintf(buf, "%d\0", a);
}

void mf_cbuf_rfill(char *buf, size_t len, char c, size_t n) {
    assert(len > n);
    size_t size = mf_strlen(buf);
    if (size < n) {
        for (size_t i = 0; i < n - size; ++i) {
            buf[size + i] = c;
        }
        buf[n] = '\0';
    }
}

char* mf_strdup(const char* a) {
    char* res = strdup(a);
    return res;
}

inline
void mf_strcpy(char* dest, const char* source) {
    strcpy(dest, source);
}

#endif //MF_STRING_IMPLEMENTATION
#endif // MF_STRING_H
