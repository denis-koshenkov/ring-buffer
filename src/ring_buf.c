#include <string.h>

#include "ring_buf.h"
#include "ring_buf_private.h"

uint8_t ring_buf_create(RingBuf *const inst, const RingBufInitCfg *const cfg)
{
    *inst = cfg->get_inst_buf(cfg->get_inst_buf_user_data);

    (*inst)->buffer = (uint8_t *)cfg->buffer;
    (*inst)->elem_size = cfg->elem_size;
    (*inst)->num_elems = cfg->num_elems;
    (*inst)->head = 0;
    (*inst)->tail = 0;
    return RING_BUF_RESULT_CODE_OK;
}

uint8_t ring_buf_push(RingBuf self, const void *const element)
{
    memcpy(self->buffer + (self->head * self->elem_size), element, self->elem_size);
    self->head = (self->head + 1) % self->num_elems;
    return RING_BUF_RESULT_CODE_OK;
}

uint8_t ring_buf_pop(RingBuf self, void *const element)
{
    memcpy(element, self->buffer + (self->tail * self->elem_size), self->elem_size);
    self->tail = (self->tail + 1) % self->num_elems;
    return RING_BUF_RESULT_CODE_OK;
}
