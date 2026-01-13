#ifndef SRC_RING_BUF_PRIVATE_H
#define SRC_RING_BUF_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

struct RingBufStruct {
    /** Buffer to hold the elements. uint8_t so that it is easy to do byte pointer arithmetic on it. */
    uint8_t *buffer;
    /** Size of one element in bytes. */
    size_t elem_size;
    /** Maximum number of elements that can be in the buffer at the same time. */
    size_t num_elems;
    size_t head;
    size_t tail;
    bool full;
};

#ifdef __cplusplus
}
#endif

#endif /* SRC_RING_BUF_PRIVATE_H */
