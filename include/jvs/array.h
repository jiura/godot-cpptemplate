#ifndef JVS_ARRAY_H
#define JVS_ARRAY_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t count;
    size_t cap;
} JvsArrHeader;

#define JVS_ARR(type) type *

#define JVS_ARR_INIT_CAP 64

#define JVS_ARR_HEADER(arr) ((JvsArrHeader *)(arr) - 1)

#define jvs_arrCount(arr) JVS_ARR_HEADER(arr)->count

static inline void jvs_arrFree(void *arr) {
    if (!arr)
        return;

    free(JVS_ARR_HEADER(arr));
}

#define jvs_arrPushFront(arr, item)                                                                          \
    do {                                                                                                     \
        JvsArrHeader *header = NULL;                                                                         \
                                                                                                             \
        if (!arr) {                                                                                          \
            header        = (JvsArrHeader *)malloc(sizeof(*(arr)) * JVS_ARR_INIT_CAP +                     \
                                                     sizeof(JvsArrHeader));                                  \
            header->count = 0;                                                                               \
            header->cap   = JVS_ARR_INIT_CAP;                                                                \
            (arr)         = (decltype(arr))(header + 1);                                                     \
        } else {                                                                                             \
            header = JVS_ARR_HEADER(arr);                                                                    \
        }                                                                                                    \
                                                                                                             \
        if (header->count >= header->cap) {                                                                  \
            header->cap *= 1.5;                                                                              \
            header = (JvsArrHeader *)realloc(header, sizeof(*(arr)) * header->cap + sizeof(JvsArrHeader)); \
            (arr)  = (decltype(arr))(header + 1);                                                            \
        }                                                                                                    \
                                                                                                             \
        memmove((arr) + 1, (arr), header->count++ * sizeof(*arr));                                           \
        *(arr) = item;                                                                                       \
    } while (0);

#define jvs_arrPushBack(arr, item)                                                                           \
    do {                                                                                                     \
        JvsArrHeader *header = NULL;                                                                         \
        if (!arr) {                                                                                          \
            header        = (JvsArrHeader *)malloc(sizeof(*(arr)) * JVS_ARR_INIT_CAP +                     \
                                                     sizeof(JvsArrHeader));                                  \
            header->count = 0;                                                                               \
            header->cap   = JVS_ARR_INIT_CAP;                                                                \
            (arr)         = (decltype(arr))(header + 1);                                                     \
        } else {                                                                                             \
            header = JVS_ARR_HEADER(arr);                                                                    \
        }                                                                                                    \
                                                                                                             \
        if (header->count >= header->cap) {                                                                  \
            header->cap *= 1.5;                                                                              \
            header = (JvsArrHeader *)realloc(header, sizeof(*(arr)) * header->cap + sizeof(JvsArrHeader)); \
            (arr)  = (decltype(arr))(header + 1);                                                            \
        }                                                                                                    \
                                                                                                             \
        (arr)[header->count++] = (item);                                                                     \
    } while (0)

#define jvs_arrPopFront(arr)                                                            \
    {                                                                                   \
        memmove((arr), (arr) + 1, (JVS_ARR_HEADER(arr)->count-- - 1) * sizeof(*(arr))); \
    }

#define jvs_arrPopBack(arr)           \
    do {                              \
        --JVS_ARR_HEADER(arr)->count; \
    } while (0);

#define jvs_arrRemove(arr, idx)                                                                                     \
    do {                                                                                                            \
        if ((idx) >= JVS_ARR_HEADER(arr)->count || (idx) < 0) {                                                     \
            /* Invalid index, do nothing */                                                                         \
        } else if ((idx) == JVS_ARR_HEADER(arr)->count - 1) {                                                       \
            --JVS_ARR_HEADER(arr)->count;                                                                           \
        } else {                                                                                                    \
            memmove((arr) + (idx), (arr) + (idx) + 1, (JVS_ARR_HEADER(arr)->count-- - (idx) - 1) * sizeof(*(arr))); \
        }                                                                                                           \
    } while (0);

#endif
