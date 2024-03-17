#ifndef MF_VECTOR_H
#define MF_VECTOR_H
#define vec(T) T*

typedef struct MFV__StretchyHeader {
    size_t size;
    size_t capacity;
} MFV__StretchyHeader;

#define mf__get_vec_header(v) (((MFV__StretchyHeader *) (v)) - 1)
#define mf_vec_size(v) ((v) ? mf__get_vec_header(v)->size : 0)
#define mf__vec_capacity(v) ((v) ? mf__get_vec_header(v)->capacity : 0)
#define mf__vec_full(v) (mf_vec_size(v) == mf__vec_capacity(v))
#define mf_vec_clear(v) ((v) ? (mf__get_vec_header(v)->size = 0) : 0)
#define mf_vec_destroy(v) ((v) ? (free(mf__get_vec_header(v)), (v) = NULL) : 0)
#define mf_vec_end(v) v[mf__get_vec_header(v)->size]
#define mf_vec_last(v) v[mf__get_vec_header(v)->size - 1]

#define mf__stretchy_check_and_resize(v) \
	(mf__vec_full(v) ? mfv__stretchy_grow((void **) &v, sizeof(*(v))) : 0)

#define mf_vec_for(v) \
    for (__typeof__(v[0]) *it = &v[0]; mf_vec_size(v) > 0 && it != &mf_vec_end(v); it++)

#define mf_vec_add(v) \
    (mf__stretchy_check_and_resize(v), &(v[mf__get_vec_header(v)->size++]))

#define mf_vec_push(v, e) \
    (*mf_vec_add(v) = (e), mf_vec_size(v) - 1)

#define mf_vec_delete(v, i) \
    (memmove(v + i, v + i + 1, (mf_vec_size(v) - i - 1) * sizeof(*v)), &v[--(mf__get_vec_header(v)->size)])


int mfv__stretchy_grow(void **v, size_t elementSize) {
    size_t newCapacity = (mf_vec_size(*v) + 1) * 2;
    size_t bytesToAlloc = sizeof(MFV__StretchyHeader) + newCapacity * elementSize;
    MFV__StretchyHeader *header;

    if (*v) {
        header = (MFV__StretchyHeader *) realloc(mf__get_vec_header(*v), bytesToAlloc);
    } else {
        header = (MFV__StretchyHeader *) malloc(bytesToAlloc);
        header->size = 0;
    }
    header->capacity = newCapacity;
    *v  = (char *) header + sizeof(MFV__StretchyHeader);
	return 0;
}


#endif // MF_VECTOR_H
