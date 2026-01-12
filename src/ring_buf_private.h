#ifndef SRC_RING_BUF_PRIVATE_H
#define SRC_RING_BUF_PRIVATE_H

#ifdef __cplusplus
extern "C"
{
#endif

struct RingBufStruct {
    /** Buffer to hold the elements. */
    void *buffer;
    /** Size of one element in bytes. */
    size_t elem_size;
};

#ifdef __cplusplus
}
#endif

#endif /* SRC_RING_BUF_PRIVATE_H */
