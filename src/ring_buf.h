#ifndef SRC_RING_BUF_H
#define SRC_RING_BUF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef struct RingBufStruct *RingBuf;

/**
 * @brief Gets called in @ref ring_buf_create to get buffer for a RingBuf instance.
 *
 * The implementation of this function should return a pointer to memory of size sizeof(struct RingBufStruct). All
 * private data for the created RingBuf instance will reside in that memory.
 *
 * The implementation of this function should be defined in a separate source file. That source file should include
 * ring_buf_private.h, which contains the definition of struct RingBufStruct. The implementation of this function then
 * knows at compile time the size of the buffer that it needs to provide.
 *
 * This function will be called as many times as @ref ring_buf_create is called (given that all parameters passed to
 * @ref ring_buf_create are valid). The implementation should be capable of returning buffers for that many distinct
 * instances.
 *
 * Implementation example - two statically allocated instances:
 * ```
 * void *get_inst_buf(void *user_data) {
 *     static struct RingBufStruct instances[2];
 *     static size_t idx = 0;
 *     return (idx < 2) ? (&(instances[idx++])) : NULL;
 * }
 * ```
 *
 * If the application uses dynamic memory allocation, another implementation option is to allocate sizeof(struct
 * RingBufStruct) bytes dynamically.
 *
 * @param user_data When this function is called, this parameter will be equal to the get_inst_buf_user_data field in
 * the RingBufInitConfig passed to @ref ring_buf_create.
 *
 * @return void * Pointer to buffer of size sizeof(struct RingBufStruct). If failed to get memory, should return NULL.
 */
typedef void *(*RingBufGetInstBuf)(void *user_data);

typedef struct {
    /** Function to get memory buffer for the instance. See @ref RingBufGetInstBuf. Cannot be NULL. */
    RingBufGetInstBuf get_inst_buf;
    /** User data argument to pass to the get_inst_buf function. */
    void *get_inst_buf_user_data;
    /** Size of one element in bytes. Must be > 0. */
    size_t elem_size;
    /** Maximum number of elements that can be in the buffer at the same time. Must be > 0. */
    size_t num_elems;
    /** Buffer to store the elements, must be of size (num_elems * elem_size). Cannot be NULL. */
    void *buffer;
} RingBufInitCfg;

typedef enum {
    RING_BUF_RESULT_CODE_OK,
    RING_BUF_RESULT_CODE_INVAL_ARG,
    RING_BUF_RESULT_CODE_NO_DATA,
} RingBufResultCode;

/**
 * @brief Create a ring buffer instance.
 *
 * @param[out] inst Created instance is written to this parameter.
 * @param[in] cfg Init config.
 *
 * @retval RING_BUF_RESULT_CODE_OK Successfully created instance.
 * @retval RING_BUF_RESULT_CODE_INVAL_ARG @p inst is NULL, @p cfg is NULL, or one of the fields in @p cfg is invalid.
 */
uint8_t ring_buf_create(RingBuf *const inst, const RingBufInitCfg *const cfg);

/**
 * @brief Push an element to the ring buffer.
 *
 * @param[in] self Ring buffer instance created by @ref ring_buf_create.
 * @param[in] element Element to push. Must point to a buffer of size "elem_size" bytes that was passed to the init cfg.
 * The element is copied into the buffer by value.
 *
 * @retval RING_BUF_RESULT_CODE_OK Successfully pushed an element into the buffer.
 * @retval RING_BUF_RESULT_CODE_NO_DATA Buffer is full, failed to push element.
 * @retval RING_BUF_RESULT_CODE_INVAL_ARG @p self is NULL or @p element is NULL.
 */
uint8_t ring_buf_push(RingBuf self, const void *const element);

/**
 * @brief Pop an element from the ring buffer.
 *
 * @param[in] self Ring buffer instance created by @ref ring_buf_create.
 * @param[out] element Buffer to write the popped element into. Must point to a buffer of size "elem_size" bytes that
 * was passed to the init cfg.
 *
 * @retval RING_BUF_RESULT_CODE_OK Successfully popped an element from the buffer.
 * @retval RING_BUF_RESULT_CODE_NO_DATA Buffer is empty, failed to pop element.
 * @retval RING_BUF_RESULT_CODE_INVAL_ARG @p self is NULL or @p element is NULL.
 */
uint8_t ring_buf_pop(RingBuf self, void *const element);

#ifdef __cplusplus
}
#endif

#endif /* SRC_RING_BUF_H */
