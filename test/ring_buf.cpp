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

static void *get_inst_buf_user_data = (void *)0x20;

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

static void push(void *element)
{
    uint8_t rc = ring_buf_push(ring_buf, element);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, rc);
}

static void pop(void *element)
{
    uint8_t rc = ring_buf_pop(ring_buf, element);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, rc);
}

TEST(RingBuf, PushPopUint16)
{
    uint16_t buffer;
    init_cfg.buffer = &buffer;
    init_cfg.elem_size = sizeof(uint16_t);
    init_cfg.num_elems = 1;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint16_t push_element = 0xABCD;
    push(&push_element);

    uint16_t pop_element = 0;
    pop(&pop_element);

    CHECK_EQUAL(push_element, pop_element);
}

TEST(RingBuf, PushPopUint32)
{
    uint32_t buffer;
    init_cfg.buffer = &buffer;
    init_cfg.elem_size = sizeof(uint32_t);
    init_cfg.num_elems = 1;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint32_t push_element = 0x5A5AA50F;
    push(&push_element);

    uint32_t pop_element = 0;
    pop(&pop_element);

    CHECK_EQUAL(push_element, pop_element);
}

TEST(RingBuf, PushPopThreeElemsUint8)
{
    uint8_t buffer[3];
    init_cfg.buffer = buffer;
    init_cfg.elem_size = sizeof(uint8_t);
    init_cfg.num_elems = 3;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint8_t elem_1 = 0xA1;
    uint8_t elem_2 = 0x0F;
    uint8_t elem_3 = 0xBB;
    push(&elem_1);
    push(&elem_2);
    push(&elem_3);

    uint8_t popped_elem_1 = 0;
    uint8_t popped_elem_2 = 0;
    uint8_t popped_elem_3 = 0;
    pop(&popped_elem_1);
    pop(&popped_elem_2);
    pop(&popped_elem_3);

    CHECK_EQUAL(elem_1, popped_elem_1);
    CHECK_EQUAL(elem_2, popped_elem_2);
    CHECK_EQUAL(elem_3, popped_elem_3);
}

static void push_pop_uint16(uint16_t elem)
{
    uint16_t elem_popped = 0;
    push(&elem);
    pop(&elem_popped);
    CHECK_EQUAL(elem, elem_popped);
}

TEST(RingBuf, PushPopUint16MoreThanBufSize)
{
    uint16_t buffer[2];
    init_cfg.buffer = buffer;
    init_cfg.elem_size = sizeof(uint16_t);
    init_cfg.num_elems = 2;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    push_pop_uint16(0x1234);
    push_pop_uint16(0x5678);
    push_pop_uint16(0x9ABC);
    push_pop_uint16(0xDEF0);
}

TEST(RingBuf, InitialPopFails)
{
    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint16_t elem = 0;
    uint8_t rc = ring_buf_pop(ring_buf, &elem);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc);
}

TEST(RingBuf, PushFailsWhenFull)
{
    uint32_t buffer[4];
    init_cfg.buffer = buffer;
    init_cfg.elem_size = sizeof(uint32_t);
    init_cfg.num_elems = 4;
    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint32_t elem_1 = 0x01234567;
    uint32_t elem_2 = 0x89ABCDEF;
    uint32_t elem_3 = 0x5A5A5A5A;
    uint32_t elem_4 = 0x0F0F0F0F;
    push(&elem_1);
    push(&elem_2);
    push(&elem_3);
    push(&elem_4);

    /* Buffer is now full */
    uint32_t elem_5 = 0x42;
    uint8_t rc = ring_buf_push(ring_buf, &elem_5);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc);
}

TEST(RingBuf, PushAfterFull)
{
    uint16_t buffer[2];
    init_cfg.buffer = buffer;
    init_cfg.elem_size = sizeof(uint16_t);
    init_cfg.num_elems = 2;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint16_t elem1 = 0x0385;
    uint16_t elem2 = 0xDC7A;
    uint16_t elem3 = 0xDC7A;
    uint16_t popped_elem1 = 0;
    uint16_t popped_elem2 = 0;
    uint16_t popped_elem3 = 0;

    push(&elem1);
    push(&elem2);

    /* At this point, buffer is full */
    pop(&popped_elem1);
    CHECK_EQUAL(elem1, popped_elem1);

    /* There is now one free slot - push should succeed */
    push(&elem3);
}

TEST(RingBuf, PopFailsWhenEmpty)
{
    uint8_t buffer[3];
    init_cfg.buffer = buffer;
    init_cfg.elem_size = sizeof(uint8_t);
    init_cfg.num_elems = 3;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint8_t elem1 = 0xF0;
    uint8_t elem2 = 0x1E;
    uint8_t elem3 = 0xD2;
    uint16_t popped_elem1 = 0;
    uint16_t popped_elem2 = 0;
    uint16_t popped_elem3 = 0;

    push(&elem1);
    push(&elem2);
    push(&elem3);

    pop(&popped_elem1);
    CHECK_EQUAL(elem1, popped_elem1);
    pop(&popped_elem2);
    CHECK_EQUAL(elem2, popped_elem2);
    pop(&popped_elem3);
    CHECK_EQUAL(elem3, popped_elem3);

    /* At this point, buffer is empty -> pop should fail */
    uint16_t popped_elem4;
    uint8_t rc = ring_buf_pop(ring_buf, &popped_elem4);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc);
}

TEST(RingBuf, StressTestSize1)
{
    uint8_t buffer;
    init_cfg.buffer = &buffer;
    init_cfg.elem_size = sizeof(uint8_t);
    init_cfg.num_elems = 1;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    /* Buffer initially empty, pop should fail */
    uint8_t unused1;
    uint8_t rc_1 = ring_buf_pop(ring_buf, &unused1);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_1);

    /* There is space for one element, push should succeed */
    uint8_t elem1 = 0x3C;
    push(&elem1);

    /* Buffer now full, push should fail */
    uint8_t unused2 = 0x4B;
    uint8_t rc_2 = ring_buf_push(ring_buf, &unused2);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_2);

    /* Buffer still full, push should fail */
    uint8_t unused3 = 0xA5;
    uint8_t rc_3 = ring_buf_push(ring_buf, &unused3);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_3);

    /* Pop the only element in the buffer */
    uint8_t popped_elem1 = 0;
    pop(&popped_elem1);
    CHECK_EQUAL(elem1, popped_elem1);

    /* Push another element into the buffer */
    uint8_t elem2 = 0x69;
    push(&elem2);

    /* Buffer now full, push should fail */
    uint8_t unused4 = 0x87;
    uint8_t rc_4 = ring_buf_push(ring_buf, &unused4);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_4);

    /* Pop the only element in the buffer */
    uint8_t popped_elem2 = 0;
    pop(&popped_elem2);
    CHECK_EQUAL(elem2, popped_elem2);

    /* Buffer is now empty, pop should fail */
    uint8_t unused5;
    uint8_t rc_5 = ring_buf_pop(ring_buf, &unused5);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_5);
}

TEST(RingBuf, StressTestSize3)
{
    uint8_t buffer[3];
    init_cfg.buffer = buffer;
    init_cfg.elem_size = sizeof(uint8_t);
    init_cfg.num_elems = 3;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    /* Buffer initially empty, pop should fail */
    uint8_t unused1;
    uint8_t rc_1 = ring_buf_pop(ring_buf, &unused1);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_1);

    /* Buffer still empty, pop should fail */
    uint8_t unused2;
    uint8_t rc_2 = ring_buf_pop(ring_buf, &unused2);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_2);

    /* Push 2 elements */
    uint8_t elem1 = 0x96;
    uint8_t elem2 = 0xAA;
    push(&elem1);
    push(&elem2);
    /* There is still one free space */

    uint8_t popped_elem1 = 0;
    pop(&popped_elem1);
    CHECK_EQUAL(elem1, popped_elem1);
    /* Now, only elem2 is left in the buffer -> 2 free spots left */

    /* Fill the two remaining slots in the buffer */
    uint8_t elem3 = 0xBB;
    uint8_t elem4 = 0xCC;
    push(&elem3);
    push(&elem4);

    /* Buffer now full, push should fail */
    uint8_t unused3 = 0x2;
    uint8_t rc_3 = ring_buf_push(ring_buf, &unused3);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_3);

    /* Pop elem2, only elem3 and elem4 are left in the buffer */
    uint8_t popped_elem2 = 0;
    pop(&popped_elem2);
    CHECK_EQUAL(elem2, popped_elem2);

    /* Fill up the buffer again */
    uint8_t elem5 = 0x7A;
    push(&elem5);
    /* elemt3, elem4, elem5 are in the buffer */

    /* Buffer full, push should fail */
    uint8_t unused4 = 0x33;
    uint8_t rc_4 = ring_buf_push(ring_buf, &unused4);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_4);

    uint8_t popped_elem3 = 0;
    pop(&popped_elem3);
    CHECK_EQUAL(elem3, popped_elem3);
    uint8_t popped_elem4 = 0;
    pop(&popped_elem4);
    CHECK_EQUAL(elem4, popped_elem4);
    /* Only elem5 is left in the buffer */

    uint8_t elem6 = 0x9A;
    push(&elem6);
    /* elem5 and elem6 are in the buffer */

    /* Empty the buffer */
    uint8_t popped_elem5 = 0;
    pop(&popped_elem5);
    CHECK_EQUAL(elem5, popped_elem5);
    uint8_t popped_elem6 = 0;
    pop(&popped_elem6);
    CHECK_EQUAL(elem6, popped_elem6);

    /* Buffer is now empty, pop should fail */
    uint8_t unused5;
    uint8_t rc_5 = ring_buf_pop(ring_buf, &unused5);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_5);

    /* Push and pop another element */
    uint8_t elem7 = 0x12;
    push(&elem7);
    uint8_t popped_elem7 = 0;
    pop(&popped_elem7);
    CHECK_EQUAL(elem7, popped_elem7);
    /* Buffer is empty again */

    /* Buffer is now empty, pop should fail */
    uint8_t unused6;
    uint8_t rc_6 = ring_buf_pop(ring_buf, &unused6);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_NO_DATA, rc_6);
}

TEST(RingBuf, PushSelfNull)
{
    uint8_t buffer;
    init_cfg.buffer = &buffer;
    init_cfg.elem_size = sizeof(uint8_t);
    init_cfg.num_elems = 1;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint8_t elem;
    uint8_t rc = ring_buf_push(NULL, &elem);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBuf, PushElementNull)
{
    uint8_t buffer;
    init_cfg.buffer = &buffer;
    init_cfg.elem_size = sizeof(uint8_t);
    init_cfg.num_elems = 1;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    uint8_t rc = ring_buf_push(ring_buf, NULL);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBuf, PopSelfNull)
{
    uint8_t buffer;
    init_cfg.buffer = &buffer;
    init_cfg.elem_size = sizeof(uint8_t);
    init_cfg.num_elems = 1;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    /* Push an element so that pop would otherwise succeed */
    uint8_t elem = 1;
    push(&elem);

    uint8_t popped_elem;
    uint8_t rc = ring_buf_pop(NULL, &popped_elem);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}

TEST(RingBuf, ElemNull)
{
    uint8_t buffer;
    init_cfg.buffer = &buffer;
    init_cfg.elem_size = sizeof(uint8_t);
    init_cfg.num_elems = 1;

    uint8_t create_rc = ring_buf_create(&ring_buf, &init_cfg);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_OK, create_rc);

    /* Push an element so that pop would otherwise succeed */
    uint8_t elem = 1;
    push(&elem);

    uint8_t rc = ring_buf_pop(ring_buf, NULL);
    CHECK_EQUAL(RING_BUF_RESULT_CODE_INVAL_ARG, rc);
}
