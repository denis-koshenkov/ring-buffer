#include "ring_buf.h"

uint8_t ring_buf_create(RingBuf *const inst, const RingBufInitCfg *const cfg)
{
    *inst = cfg->get_inst_buf(cfg->get_inst_buf_user_data);
    return RING_BUF_RESULT_CODE_OK;
}
