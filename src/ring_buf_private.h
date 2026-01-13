#ifndef SRC_RING_BUF_PRIVATE_H
#define SRC_RING_BUF_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

struct RingBufStruct {
    /** Buffer to hold the elements. uint8_t so that it is easy to do byte pointer arithmetic on it. */
    uint8_t *buffer;
    /** Size of one element in bytes. */
    size_t elem_size;
    size_t head;
    size_t tail;
};

#ifdef __cplusplus
}
#endif

#endif /* SRC_RING_BUF_PRIVATE_H */
