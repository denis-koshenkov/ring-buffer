# Ring Buffer
A lightweight ring buffer implementation in C intended to be used in embedded systems.

This library was developed with Test-Driven Development (TDD) using CppUTest framework.

# Usage
```c
/* Create a ring buffer instance */
static uint32_t buf[3];
RingBufInitCfg init_cfg = {
    /* You need to implement this function, see below on how */
    .get_inst_buf = get_inst_buf,
    /* Optional user data to pass to get_inst_buf function */ 
    .get_inst_buf_user_data = NULL,
    /* Size of one element in bytes */
    .elem_size = sizeof(uin32_t),
    /* Maximum number of elements that can be stored in the buffer at the same time */ 
    .num_elems = 3,
    /* Memory buffer to store the elements, must be of size (num_elems * elem_size) bytes */
    .buffer = buf,
};
RingBuf inst;
uint8_t create_rc = ring_buf_create(&inst, &init_cfg);
if (create_rc != RING_BUF_RESULT_CODE_OK) {
    // Error handling
}

/* Push three elements to the buffer */
uint8_t push_rc;
uint32_t elem1 = 10;
push_rc = ring_buf_push(inst, &elem1);
if (push_rc != RING_BUF_RESULT_CODE_OK) {
    // Error handling
}

uint32_t elem2 = 20;
push_rc = ring_buf_push(inst, &elem2);
if (push_rc != RING_BUF_RESULT_CODE_OK) {
    // Error handling
}

uint32_t elem3 = 30;
push_rc = ring_buf_push(inst, &elem3);
if (push_rc != RING_BUF_RESULT_CODE_OK) {
    // Error handling
}

/* Buffer is now full, this push will fail */
uint32_t unused = 40;
push_rc = ring_buf_push(inst, &unused);
/* push_rc == RING_BUF_RESULT_CODE_NO_DATA - failed to push element because buffer was full */

/* Pop an element from the buffer */
uint8_t pop_rc;
uint32_t popped_elem1;
pop_rc = ring_buf_pop(inst, &popped_elem1);
if (pop_rc != RING_BUF_RESULT_CODE_OK) {
    // Error handling
}
/* popped_elem1 == 10 */
```

## Get inst buf function
`get_inst_buf` function that is passed to init cfg must return a memory buffer that will be used for private data of a ring buffer instance. The memory buffer must remain valid as long as the instance is being used.

This function is called once per instance as a part of `ring_buf_create`.

There are two main ways to implement this function:
1) Return pointer to statically allocated memory. If you need one ring buffer instance:
```c
void *get_inst_buf(void *user_data) {
    static struct RingBufStruct inst;
    return &inst;
}
```
If you need several ring buffer instances:
```c
#define RING_BUFFER_NUM_INSTANCES 2

void *get_inst_buf(void *user_data) {
    static struct RingBufStruct instances[RING_BUFFER_NUM_INSTANCES];
    static size_t idx = 0;
    return (idx < RING_BUFFER_NUM_INSTANCES) ? (&(instances[idx++])) : NULL;
}
```

2) Dynamically allocate memory for an instance:
```c
void *get_inst_buf(void *user_data) {
    return malloc(sizeof(struct RingBufStruct));
}
```

If you are using option 2, then you need to free the memory when the ring buffer instance is destroyed. For this, a `ring_buf_destroy` function needs to be implemented - this is future work, not implemented at the moment.  

`struct RingBufStruct` is a data type that defines the private data of a ring buffer instance. It is defined in the `ring_buffer_private.h` file.

That header must not be included by ring buffer module users. The only exception is a module that implements `get_inst_buf`, because the implementation of that function needs to know about `struct RingBufStruct`, so that it knows the size of the memory buffer it needs to return.

It is recommended to define the implementation of `get_inst_buf` in a separate .c file and do `#include "ring_buf_private.h"` only in that .c file.

This way, other modules that interact with the ring buffer module do not have to include `ring_buf_private.h`, which means they cannot access private data of the ring buffer instance directly.

# Integration Details
Add the following to your build:
- `src/ring_buf.c` source file
- `src` directory as include directory

# Running Tests
Follow these steps in order to run all unit tests for the ring buffer source code.

Prerequisites:
- CMake
- C compiler (e.g. gcc)
- C++ compiler (e.g. g++)

Steps:
1. Clone this repository
2. Fetch the CppUTest submodule:
```
git submodule init
git submodule update
```
3. Build and run the tests:
```
./run_tests.sh
```
