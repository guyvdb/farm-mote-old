#pragma once

#include <stddef.h>
#include <stdint.h>

#include "frame.h"

#define FRAMEBUF_DATA_LEN 512


// The framebuf is a circular buffer. Data is copied from the network stream buffer into the framebuf.
// The framebuf is used to unfragment potentially fragmented frame bytes and then deframe them into frames.
// There is only one framebuf defined for the system. It is defined in framebuf.c as static.
struct framebuf {
  uint8_t data[FRAMEBUF_DATA_LEN];
  size_t head;
  size_t tail;
};
typedef struct framebuf framebuf_t;


// Reset the head and tail of the framebuf to 0. This zeros the buffer.
// All data that was in the buffer is lost.
void framebuf_reset(void);

// Put a single byte into the framebuf.
// data - the byte to put in the buffer
// return - 1 on success or 0 on failure
int framebuf_put(uint8_t data);

// Write an array of bytes to the framebuf.
// data - the array of bytes
// len - the size of the array
// return - 1 on success or 0 on failure
int framebuf_write(uint8_t *data, size_t len);

// How much data is currently in the framebuf. What
// is the total byte count.
// return - the number of bytes in use.
int framebuf_size(void);

// Deframe the next complete frame. The memory for the new
// frame is allocated. The caller is responsiable for calling
// frame_free() at some point.
// return - a newly allocated frame or null if no frame exists
frame_t *framebuf_deframe(void);
