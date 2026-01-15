#include <string.h>
#include <stdbool.h>

#include "ring_buf.h"
#include "ring_buf_private.h"

/**
 * @brief Check whether init config is valid.
 *
 * @param cfg Init config.
 *
 * @retval true Init config is valid.
 * @retval false Init config is invalid.
 */
static bool is_valid_cfg(const RingBufInitCfg *const cfg)
{
    // clang-format off
    return (
        cfg
        && cfg->get_inst_buf
        && (cfg->elem_size > 0)
        && (cfg->num_elems > 0)
        && cfg->buffer
    );
    // clang-format on
}

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
    if (!inst || !is_valid_cfg(cfg)) {
        return RING_BUF_RESULT_CODE_INVAL_ARG;
    }

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
    self->full = false;
    return RING_BUF_RESULT_CODE_OK;
}
