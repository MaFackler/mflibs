#ifndef MF_STRING_H
#define MF_STRING_H
#include <ctype.h>
#include <assert.h>

#define S(d) mf_strview {d, strlen(d)}

struct mf_strview {
    const char *data; 
    u64 size;
};

struct mf_str {
    char *data;
    u64 size;
    u64 capacity;
};

#define mf_str_stack(n) mf_str{(char *) alloca(n), 0, n}
inline mf_str mf_str_new(size_t n);
inline void mf_str_free(mf_str s);

#ifdef MF_STRING_IMPLEMENTATION

mf_str mf_str_new(size_t n) {
    mf_str res;
    res.data = (char *) calloc(n, 1);
    res.size = 0;
    res.capacity = n;
    return res;
}

inline void mf_str_free(mf_str s) {
    free(s.data);
    s.size = 0;
    s.capacity = 0;
}

#endif

#if 0

// Allocators
#ifndef mf_str_malloc
    #define mf_str_malloc malloc
#endif
#ifndef mf_str_memfree
    #define mf_str_memfree free
#endif
#ifndef mf_str_realloc
    #define mf_str_realloc realloc
#endif



#define mf_str_stack(size) mf_str{(char *) alloca(size), size}

void mf_str_free(char *a);
char* mf_str_new(size_t n) {
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

// TODO: figure out a good naming for c strings (char *) and selfmade strings mf_string
struct mf_string {
    char *data;
    u32 size;
    u32 capacity;
};

mf_string mf_string_new(size_t capacity);




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
    char* neww = (char *) mf_str_realloc(*a, sizeA + sizeB + 1);
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

mf_string mf_string_new(size_t capacity) {
    mf_string res;
    res.data = mf_str_new(size);
    res.capacity = size;
    res.size = 0;
    return res;
}

#endif //MF_STRING_IMPLEMENTATION

#ifdef MF_TEST_ACTIVE

TEST("mf_string_endswith") {
    const char* my_base_string = "hello";
    MFT_CHECK(!mf_str_endswith(my_base_string, "ll"));
    MFT_CHECK(mf_str_endswith(my_base_string, "llo"));
}

TEST("mf_string_is_substring") {
    const char* my_base_string = "hello";
    MFT_CHECK(!mf_str_is_substr(my_base_string, "hllo"));
    MFT_CHECK(mf_str_is_substr(my_base_string, "hel"));
}

TEST("mf_string_is_equal") {
    const char* my_base_string = "hello";
    MFT_CHECK(!mf_str_is_equal((char *) my_base_string, "hllo"));
    MFT_CHECK(mf_str_is_equal((char *) my_base_string, "hello"));
    MFT_CHECK(!mf_str_is_equal((char *) my_base_string, "helloo"));
    MFT_CHECK(!mf_str_is_equal((char *) my_base_string, "hell"));
}

TEST("mf_string_count_char") {
    const char* my_base_string = "hello";
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'l'), 2);
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'e'), 1);
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'o'), 1);
}

TEST("mf_str_is_substr") {
    MFT_CHECK_TRUE(mf_str_is_substr("abc", "ab"));
    MFT_CHECK_FALSE(mf_str_is_substr("abc", "ac"));
}

TEST("mf_str_is_fuzzy") {
    MFT_CHECK_TRUE(mf_str_is_fuzzy("abc", "ac"));
    MFT_CHECK_FALSE(mf_str_is_fuzzy("abc", "ad"));

    MFT_CHECK_TRUE(mf_str_is_fuzzy("Abc", "ac"));
    MFT_CHECK_TRUE(mf_str_is_fuzzy("main.cpp", "mainc"));
}

TEST("mf_to_cbuf") {
    char buf[16];
    mf_to_cbuf(buf, 20);
    MFT_CHECK_STRING(buf, "20");
}

TEST("mf_to_cbuf") {
    char buf[16];
    mf_to_cbuf(buf, 20);
    MFT_CHECK_STRING(buf, "20");
    mf_cbuf_rfill(buf, sizeof(buf), '-', 4);
    MFT_CHECK_STRING(buf, "20--");
}

#endif
#endif 

#endif // MF_STRING_H
