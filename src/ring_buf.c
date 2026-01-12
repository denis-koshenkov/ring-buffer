#include <string.h>

#include "ring_buf.h"
#include "ring_buf_private.h"

uint8_t ring_buf_create(RingBuf *const inst, const RingBufInitCfg *const cfg)
{
    *inst = cfg->get_inst_buf(cfg->get_inst_buf_user_data);
    return RING_BUF_RESULT_CODE_OK;
}

uint8_t ring_buf_push(RingBuf self, const void *const element)
{
    memcpy(&self->element, element, sizeof(uint16_t));
    return RING_BUF_RESULT_CODE_OK;
}

uint8_t ring_buf_pop(RingBuf self, void *const element)
{
    *(uint16_t *)element = self->element;
    return RING_BUF_RESULT_CODE_OK;
}
