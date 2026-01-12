#ifndef TEST_MOCK_MOCK_CFG_FUNCTIONS_H
#define TEST_MOCK_MOCK_CFG_FUNCTIONS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ring_buf.h"

void *mock_ring_buf_get_inst_buf(void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* TEST_MOCK_MOCK_CFG_FUNCTIONS_H */
