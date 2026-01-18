#include <string.h>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "ring_buf.h"
/* Included to know the size of RingBuf instance to return from mock_ring_buf_get_inst_buf. */
#include "ring_buf_private.h"
#include "mock_cfg_functions.h"

/* To return from mock_ring_buf_get_inst_buf */
static struct RingBufStruct inst_buf;

static RingBuf ring_buf;
static RingBufInitCfg init_cfg;

static void *get_inst_buf_user_data = (void *)0x30;

#define RING_BUF_TEST_DEFAULT_NUM_ELEMS 1
static uint8_t default_buffer[RING_BUF_TEST_DEFAULT_NUM_ELEMS * sizeof(uint8_t)];

static void populate_default_init_cfg(RingBufInitCfg *const cfg)
{
    cfg->get_inst_buf = mock_ring_buf_get_inst_buf;
    cfg->get_inst_buf_user_data = get_inst_buf_user_data;
    cfg->buffer = default_buffer;
    cfg->elem_size = sizeof(uint8_t);
    cfg->num_elems = RING_BUF_TEST_DEFAULT_NUM_ELEMS;
}

// clang-format off
TEST_GROUP(RingBufNoSetup){
    void setup() {
        ring_buf = NULL;
        memset(&init_cfg, 0, sizeof(RingBufInitCfg));
        memset(&inst_buf, 0, sizeof(struct RingBufStruct));

        populate_default_init_cfg(&init_cfg);
    }
};
// clang-format on

TEST(RingBufNoSetup, CreateReturnsInvalArgInstNull)
{
    uint8_t rc = ring_buf_create(NULL, &init_cfg);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBufNoSetup, CreateReturnsInvalArgCfgNull)
{
    uint8_t rc = ring_buf_create(&ring_buf, NULL);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBufNoSetup, CreateGetInstBufNull)
{
    init_cfg.get_inst_buf = NULL;
    uint8_t rc = ring_buf_create(&ring_buf, &init_cfg);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBufNoSetup, CreateElemSize0)
{
    init_cfg.elem_size = 0;
    uint8_t rc = ring_buf_create(&ring_buf, &init_cfg);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBufNoSetup, CreateNumElems0)
{
    init_cfg.num_elems = 0;
    uint8_t rc = ring_buf_create(&ring_buf, &init_cfg);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBufNoSetup, CreateBufferNull)
{
    init_cfg.buffer = NULL;
    uint8_t rc = ring_buf_create(&ring_buf, &init_cfg);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBufNoSetup, CreateGetInstBufReturnsNull)
{
    mock()
        .expectOneCall("mock_ring_buf_get_inst_buf")
        .withParameter("user_data", get_inst_buf_user_data)
        .andReturnValue((void *)NULL);

    uint8_t rc = ring_buf_create(&ring_buf, &init_cfg);

    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc);
}
