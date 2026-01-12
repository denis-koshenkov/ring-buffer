add_library(ring_buf INTERFACE)

target_sources(ring_buf INTERFACE
    ring_buf.c
)

target_include_directories(ring_buf INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
