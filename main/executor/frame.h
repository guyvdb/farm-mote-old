#pragma once

#include "freertos/FreeRTOS.h"

// A frame can be extracted from a set of bytes. A frame can also be turned into a set of bytes.
// Frames contain commands. Frames are sent from client to server or vise versa.

// Data in the frame is in big endian or network byte order.

#define FRAMEVERSION 0x1


#define FRAMEBUFLEN 256 // how big is the ring buffer for extracting frames 
#define ESCAPE 0x1B
#define SFLAG 0x2
#define EFLAG 0x3



// The framebuf is a circular buffer. Data is copied from the network stream buffer into the framebuf.
// The framebuf is used to unfragment potentially fragmented frame bytes and then deframe them into whole
// whole frame bytes
struct framebuf {
  uint8_t data[FRAMEBUFLEN];
  size_t head;
  size_t tail;
};
typedef struct framebuf framebuf_t;


// Structure of a frame on the wire
//
// |--------------|---------------|------------|-------------------|---------------------|---------------|------------|-----------|--------------------|--------------|
// | SFLAG 1 byte | version 1byte | id 2 bytes | retransmit 1 byte | transmitted 4 bytes | refid 2 bytes | cmd 1 byte | len 1byte | payload <varbytes> | EFLAG 1 byte |
// |--------------|---------------|------------|-------------------|---------------------|---------------|------------|-----------|--------------------|--------------|
//
// The SFLAG and EFLAG are not part of the frame. 
// The frame is 16 bytes + payload size
// The transmitted bytes are 18 bytes + payload size
// The meaning of payload is individually defined by each command
//
struct frame {

  // A pointer to a next frame if this frame is being stored in a linked list
  struct frame *next;
  
  // A pointer to the previous frame if this frame is being stored in a linked list 
  struct frame *prev;
  
  // The version of the frame protocol.
  uint8_t version;
  
  // The id of this frame generated by the client or the server. Client and server ids have no
  // shared sequence association between them. Id increments but is not monotonic. It wrapps and
  // repeats the sequence when limit is reached. The id should never be set to 0x0. It should be
  // set in the range 1 to max size. The counter can be reset at any time, such as on reboot.
  uint16_t id;

  // transmission count -- how many times has this frame been sent 
  uint8_t tcount;

  // This is a time stamp of when this frame was transmitted. It is seconds only. It is tv_sec
  // of struct timeval. The value is signed 32 bit integer big endian encoded.
  int32_t transmitted;

  // This is the reference frame id. A frame that this frame is in response to. This could be
  // set to 0x0 if this frame does not reference another frame.
  uint16_t refid;

  // This is the command identifier of this frame
  uint8_t cmd;

  // This is the length of the payload of this frame. The maximum is 255 bytes
  uint8_t len;

  // This is the payload of this frame.
  uint8_t *payload;
};
typedef struct frame frame_t;


// A framelst is used for storing frames in a linked list
struct framelst {
  frame_t *root;
};
typedef struct framelst framelst_t;

// A function to be called when 
typedef void (frame_iterate)(char *argv[], int argc);



// Create a frame. Memory is allocated for the frame and len bytes is allocated for the payload
frame_t *frame_create(uint8_t *payload, uint8_t len);

// Create a frame from a set of bytes that includes the SFRAME and EFRAME flags that are not part
// of the frame. Memory is allocated via frame_create().
frame_t *frame_from_bytes(uint8_t *data, size_t len);

// Free a frame. Free the frame struct memory and the memory that was allocated for *payload.
void frame_free(frame_t *frame);

// Print a decoded frame 
void frame_print(frame_t *frame, uint8_t *bytes, size_t size);

// Reset the head and tail of the framebuf to 0
void framebuf_reset();

// Put a single byte into the framebuf
int framebuf_put(uint8_t data);

// Write an array of bytes to the framebuf 
int framebuf_write(uint8_t *data, size_t len);

// How much data is currently in the framebuf
int framebuf_size();

// What is the size of the first full frame in the buffer 
int framebuf_frame_size();

// Deframe the bytes of a single frame from the buffer 
int framebuf_deframe(uint8_t *data, size_t len);

// Create a framelst
framelst_t *framelst_create();

// Free a frame list. This does not free any memory associated with
// frames. It only frees the list memory
void framelst_free(framelst_t *lst);

// Add a frame to the frame list
void framelst_add(framelst_t *lst, frame_t *frame);

// Remove a frame from a framelst. This does NOT free the frame memory.
int framelst_remove(framelst_t *lst, frame_t *frame);

// Find a frame by id in a framelst
frame_t *framelst_find_frame_by_id(framelst_t *lst, uint8_t id);

// Find a frame by refid in a framelst
frame_t *framelst_find_frame_by_refid(framelst_t *lst, uint8_t refid);