#include <string.h>

#include "ring_buf.h"
#include "ring_buf_private.h"

/**
 * @brief Check whether ring buffer is empty.
 *
 * @param[in] self Ring buffer instance.
 *
 * @retval true Ring buffer is empty.
 * @retval false Ring buffer is not empty.
 */
static bool is_empty(RingBuf self)
{
    return ((self->head == self->tail) && !self->full);
}

/**
 * @brief Check whether ring buffer is full.
 *
 * @param[in] self Ring buffer instance.
 *
 * @retval true Ring buffer is full.
 * @retval false Ring buffer is not full.
 */
static bool is_full(RingBuf self)
{
    return self->full;
}

uint8_t ring_buf_create(RingBuf *const inst, const RingBufInitCfg *const cfg)
{
    *inst = cfg->get_inst_buf(cfg->get_inst_buf_user_data);

    (*inst)->buffer = (uint8_t *)cfg->buffer;
    (*inst)->elem_size = cfg->elem_size;
    (*inst)->num_elems = cfg->num_elems;
    (*inst)->head = 0;
    (*inst)->tail = 0;
    (*inst)->full = false;
    return RING_BUF_RESULT_CODE_OK;
}

uint8_t ring_buf_push(RingBuf self, const void *const element)
{
    if (is_full(self)) {
        return RING_BUF_RESULT_CODE_NO_DATA;
    }

    memcpy(self->buffer + (self->head * self->elem_size), element, self->elem_size);
    self->head = (self->head + 1) % self->num_elems;
    if (self->head == self->tail) {
        self->full = true;
    }
    return RING_BUF_RESULT_CODE_OK;
}

uint8_t ring_buf_pop(RingBuf self, void *const element)
{
    if (is_empty(self)) {
        return RING_BUF_RESULT_CODE_NO_DATA;
    }

    memcpy(element, self->buffer + (self->tail * self->elem_size), self->elem_size);
    self->tail = (self->tail + 1) % self->num_elems;
    return RING_BUF_RESULT_CODE_OK;
}
