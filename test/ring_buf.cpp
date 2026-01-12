#include <string.h>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "ring_buf.h"
/* Included to know the size of RingBuf instance to return from mock_ring_buf_get_inst_buf. */
#include "ring_buf_private.h"
#include "mock_cfg_functions.h"

/* To return from mock_ring_buf_get_inst_buf */
static struct RingBufStruct inst_buf;

RingBuf ring_buf;
RingBufInitCfg init_cfg;

void *get_inst_buf_user_data = (void *)0x20;

static void populate_default_init_cfg(RingBufInitCfg *const cfg)
{
    cfg->get_inst_buf = mock_ring_buf_get_inst_buf;
    cfg->get_inst_buf_user_data = get_inst_buf_user_data;
}

// clang-format off
TEST_GROUP(RingBuf){
    void setup() {
        ring_buf = NULL;
        memset(&init_cfg, 0, sizeof(RingBufInitCfg));
        memset(&inst_buf, 0, sizeof(struct RingBufStruct));

        /* Every test should call ring_buf_create at the beginning, which will call this mock */
        mock()
            .expectOneCall("mock_ring_buf_get_inst_buf")
            .withParameter("user_data", get_inst_buf_user_data)
            .andReturnValue((void *)&inst_buf);

        populate_default_init_cfg(&init_cfg);
    }
};
// clang-format on

TEST(RingBuf, CreateReturnsBufReturnedFromGetInstBuf)
{
    uint8_t rc = ring_buf_create(&ring_buf, &init_cfg);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, rc);
    CHECK_EQUAL((void *)&inst_buf, (void *)ring_buf);
}

TEST(RingBuf, PushPopUint16)
{
    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint16_t push_element = 0xABCD;
    uint8_t push_rc = ring_buf_push(ring_buf, &push_element);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, push_rc);

    uint16_t pop_element = 0;
    uint8_t pop_rc = ring_buf_pop(ring_buf, &pop_element);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, pop_rc);

    CHECK_EQUAL(push_element, pop_element);
}
