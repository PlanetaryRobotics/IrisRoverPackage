/**
 * @file buffer.h
 * @brief File implementing a simple ring buffer
 */
#ifndef __BUFFER_INC
#define __BUFFER_INC

#include <stdint.h>

#define BUFFER_SIZE 600

struct buffer {
    /* the actual array of characters */
    unsigned char buf[BUFFER_SIZE];

    /* the current offset into the buffer used (wraps around past BUFFER_SIZE) */
    uint16_t idx;

    /* number of bytes used in the buffer */
    uint16_t used;
};

/* all of the buffers that exist; total = 3KiB used for tx/rx buffers */
__volatile extern struct buffer pbuf, uart0rx, uart0tx, uart1rx, uart1tx;

/**
 * Allocate n bytes in the buffer x.
 */
#define buffer_alloc(x, n) (x->used += n)

#endif /* __BUFFER_INC */
