#include <string.h>

#include "ring_buf.h"
#include "ring_buf_private.h"

uint8_t ring_buf_create(RingBuf *const inst, const RingBufInitCfg *const cfg)
{
    *inst = cfg->get_inst_buf(cfg->get_inst_buf_user_data);

    (*inst)->buffer = cfg->buffer;
    (*inst)->elem_size = cfg->elem_size;
    return RING_BUF_RESULT_CODE_OK;
}

uint8_t ring_buf_push(RingBuf self, const void *const element)
{
    memcpy(self->buffer, element, self->elem_size);
    return RING_BUF_RESULT_CODE_OK;
}

uint8_t ring_buf_pop(RingBuf self, void *const element)
{
    memcpy(element, self->buffer, self->elem_size);
    return RING_BUF_RESULT_CODE_OK;
}
