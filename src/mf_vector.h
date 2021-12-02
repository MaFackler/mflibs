#ifndef MF_VECTOR_H
#define MF_VECTOR_H

/* USAGE

// StretchyBuffer
// ==================================================

int *arr = NULL;
mf_vec_push(arr, 1);
mf_vec_push(arr, 2);

for (int i = 0; i < mf_vec_size(arr); ++i) {
    printf("%d\n", arr[i]);
}

mf_vec_for(arr) {
    printf("%d\n", *it);
}

mf_vec_clear(arr);
mf_vec_destroy(arr);
*/

#ifdef MF_VECTOR_IMPLEMENTATION

typedef struct {
    u32 size;
    u32 capacity;
} mf__stretchy_header;


#define mf__get_stretchy_header(v) (((mf__stretchy_header *) (v)) - 1)
#define mf_vec_size(v) ((v) ? mf__get_stretchy_header(v)->size : 0)
#define mf__stretchy_capacity(v) ((v) ? mf__get_stretchy_header(v)->capacity : 0)
#define mf__stretchy_full(v) (mf_vec_size(v) == mf__stretchy_capacity(v))
#define mf_vec_clear(v) ((v) ? (mf__get_stretchy_header(v)->size = 0) : 0)
#define mf_vec_destroy(v) ((v) ? (free(mf__get_stretchy_header(v)), (v) = NULL) : 0)
#define mf_vec_end(v) v[mf__get_stretchy_header(v)->size]
#define mf_vec_last(v) v[mf__get_stretchy_header(v)->size - 1]
#define mf_vec_safe_index_ptr(v, i) arr != NULL ? &arr[i] : NULL

#define mf__stretchy_check_and_resize(v) \
	(mf__stretchy_full(v) ? mf__stretchy_grow((void **) &v, sizeof(*(v))) : 0)

#define mf_vec_for(v) \
    for (auto it = &v[0]; mf_vec_size(v) > 0 && it != &mf_vec_end(v); it++)

// TODO: mf_vec_addn

#define mf_vec_add(v) \
    (mf__stretchy_check_and_resize(v), &(v)[mf__get_stretchy_header(v)->size++])

#define mf_vec_push(v, e) \
    *mf_vec_add(v) = e


inline int mf__stretchy_grow(void **v, size_t elementSize) {
    size_t newCapacity = (mf_vec_size(*v) + 1) * 2;
    size_t bytesToAlloc = sizeof(mf__stretchy_header) + newCapacity * elementSize;
    mf__stretchy_header *header;

    if (*v) {
        header = (mf__stretchy_header *) realloc(mf__get_stretchy_header(*v), bytesToAlloc);
    } else {
        header = (mf__stretchy_header *) malloc(bytesToAlloc);
        header->size = 0;
    }
    header->capacity = newCapacity;
    *v  = (char *) header + sizeof(mf__stretchy_header);
	return 0;
}

#endif // MF_VECTOR_IMPLEMENTATION

#endif // MF_VECTOR_H
