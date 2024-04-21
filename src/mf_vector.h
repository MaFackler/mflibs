#ifndef MF_VECTOR_H
#define MF_VECTOR_H
#define vec(T) T*
#include <stdlib.h>
#include <stddef.h>

typedef struct MFV__StretchyHeader {
    size_t size;
    size_t capacity;
} MFV__StretchyHeader;

#define MF_VecHeader(v) (((MFV__StretchyHeader *) (v)) - 1)
#define MF_VecLen(v) ((v) ? MF_VecHeader(v)->size : 0)
#define MF_VecCapacity(v) ((v) ? MF_VecHeader(v)->capacity : 0)
#define MF_VecIsFull(v) (MF_VecLen(v) == MF_VecCapacity(v))
#define MF_VecClear(v) ((v) ? (MF_VecHeader(v)->size = 0) : 0)
#define MF_VecDestroy(v) ((v) ? (free(MF_VecHeader(v)), (v) = NULL) : 0)
#define MF_VecEnd(v) v[MF_VecHeader(v)->size]
#define MF_VecLast(v) v[MF_VecHeader(v)->size - 1]
#define MF_VecFor(v) for (__typeof__(v[0]) *it = &v[0]; MF_VecLen(v) > 0 && it != &MF_VecEnd(v); it++)
#define MF_VecAdd(v) (MFV__CheckAndResize(v), &(v[MF_VecHeader(v)->size++]))
#define MF_VecPush(v, e) (*MF_VecAdd(v) = (e), MF_VecLen(v) - 1)
#define MF_VecDelete(v, i) (memmove(v + i, v + i + 1, (MF_VecLen(v) - i - 1) * sizeof(*v)), &v[--(MF_VecHeader(v)->size)])
#define MFV__CheckAndResize(v) (MF_VecIsFull(v) ? MFV__VecGrow((void **) &v, sizeof(*(v))) : 0)

static inline int MFV__VecGrow(void **v, size_t elementSize) {
    size_t newCapacity = (MF_VecLen(*v) + 1) * 2;
    size_t bytesToAlloc = sizeof(MFV__StretchyHeader) + newCapacity * elementSize;
    MFV__StretchyHeader *header;

    if (*v) {
        header = (MFV__StretchyHeader *) realloc(MF_VecHeader(*v), bytesToAlloc);
    } else {
        header = (MFV__StretchyHeader *) malloc(bytesToAlloc);
        header->size = 0;
    }
    header->capacity = newCapacity;
    *v  = (char *) header + sizeof(MFV__StretchyHeader);
	return 0;
}


#endif // MF_VECTOR_H
