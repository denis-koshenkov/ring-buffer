#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "ring_buf.h"
/* Included to know the size of RingBuf instance to return from mock_ring_buf_get_inst_buf. */
#include "ring_buf_private.h"
#include "mock_cfg_functions.h"

/* To return from mock_ring_buf_get_inst_buf */
static struct RingBufStruct inst_buf;

TEST_GROUP(RingBuf){};

TEST(RingBuf, CreateReturnsBufReturnedFromGetInstBuf)
{
    void *get_inst_buf_user_data = (void *)0x20;
    mock()
        .expectOneCall("mock_ring_buf_get_inst_buf")
        .withParameter("user_data", get_inst_buf_user_data)
        .andReturnValue((void *)&inst_buf);

    RingBuf ring_buf;
    RingBufInitCfg cfg = {
        .get_inst_buf = mock_ring_buf_get_inst_buf,
        .get_inst_buf_user_data = get_inst_buf_user_data,
    };
    uint8_t rc = ring_buf_create(&ring_buf, &cfg);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, rc);
    CHECK_EQUAL((void *)&inst_buf, (void *)ring_buf);
}
