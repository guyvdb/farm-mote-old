#include "frame.h"
#include "bytes.h"


#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>


static framebuf_t buffer; // the one and only framebuf circular buffer 



/* ========================================================================
 * ============================== FRAME  ==================================
 * ======================================================================== */



/* ------------------------------------------------------------------------
 * Allocate a frame structure. Also allocate space for the payload, if any.
 * The caller must free the memory.
 * --------------------------------------------------------------------- */
frame_t *frame_create(uint8_t *payload, uint8_t len) {
  frame_t *frame = malloc(sizeof(frame_t));
  frame->len = len;
  frame->next = 0x0;
  frame->prev = 0x0;
  
  
  if (len > 0) {
    int plen = (int)len;
    frame->payload = malloc(plen);
    
    for(int i=0;i<plen;i++) {
      frame->payload[i] = payload[i];
    }    
  } else {
    frame->payload = 0x0;
  }

  return frame;
  
}


/* ------------------------------------------------------------------------
 * Allocate a frame structure. Populate it with the data bytes. Also allocate
 * and populate the payload, if any. The caller must free the memory.
 * --------------------------------------------------------------------- */
frame_t *frame_from_bytes(uint8_t *data, size_t len) {
  int flen;
  uint8_t *ptr = data;

  // alloc frame memory 
  frame_t *frame = malloc(sizeof(frame_t));

  frame->next = 0x0;
  frame->prev = 0x0;

  // skip SFLAG - increment 1 byte
  ptr++;

  // version
  frame->version = bytes_uint8_decode(ptr); ptr += 1;
  
  // id
  frame->id =  bytes_uint16_decode(ptr);  ptr += 2;

  // tcount
  frame->tcount = bytes_uint8_decode(ptr); ptr += 1;

  // transmitted timestamp
  frame->transmitted = (int32_t)bytes_uint32_decode(ptr); ptr += 4;

  // ref id
  frame->refid = bytes_uint16_decode(ptr); ptr += 2;

  // cmd
  frame->cmd = bytes_uint8_decode(ptr); ptr += 1;

  // len
  frame->len = bytes_uint8_decode(ptr); ptr += 1;
  flen = (int)frame->len;


  // if we have any payload data alloc or else payload is set to null.
  if(flen > 0) {
    // Alloc the payload array
    frame->payload = malloc(flen);
  
    // copy the payload
    for(int i=0;i<flen;i++) {
      frame->payload[i] = ptr[i];
    }
  } else {
    frame->payload = 0x0;
  }
  
  return frame;
}

/* ------------------------------------------------------------------------
 * Free the memory allocated for the frame and the payload.
 * --------------------------------------------------------------------- */
void frame_free(frame_t *frame) {

  // free the payload if it was allocated
  if(frame->payload != 0x0) {
    free(frame->payload);
  }

  // free the frame
  free(frame);
}


/* ------------------------------------------------------------------------
 * Print this frame 
 * --------------------------------------------------------------------- */
void frame_print(frame_t *frame, uint8_t *bytes, size_t size) {
  int len = (int)frame->len;


  printf("\n-- FRAME --\n");

  printf("version: %d\n", frame->version);
  printf("id: %d\n", frame->id);
  printf("tcount: %d\n", frame->tcount);
  printf("transmitted: %d\n", frame->transmitted);
  printf("refid: %d\n", frame->refid);
  printf("cmd: %d\n", frame->cmd);
  printf("len: %d\n", frame->len);

  printf("Payload:\n");
  if (frame->payload != 0x0) {
    printf("  [");
    for(int i=0;i<len;i++) {
      printf("%d ", frame->payload[i]);
    }
    printf("]\n");
  } else {
    printf("  [NULL]\n");
  }


  printf("Network Bytes:\n");
  printf("  [");

  for(int i = 0; i <size; i++) {
    printf("%d ", bytes[i]);
  }
  printf("] Size=%d\n", size);


  
  printf("-- END FRAME --\n\n");
}



/* ========================================================================
 * ============================ FRAMEBUF  =================================
 * ======================================================================== */



/* ------------------------------------------------------------------------
 * find the index in the circular buffer of first occurence of nonescaped 
 * data starting from head and proceeding to tail. return index or -1
 * --------------------------------------------------------------------- */
static int index(uint8_t data) {
  size_t size = framebuf_size();
  int ptr = buffer.head;
  int escape = 0;

  
  
  for(int i=0;i<size;i++) {
    if(buffer.data[ptr] == ESCAPE) {
      escape = 1;
    } else {
      if(buffer.data[ptr] == data && escape != 1) {
        return ptr;
      }
      escape = 0;
    }

    if(ptr + 1 == buffer.tail) {
      // we have searched the whole buffer
      return -1;
    }
    

    // increment the pointer
    ptr++;

    // if ptr has reached the end of the array set it to the beginning
    if (ptr >= FRAMEBUFLEN) {
      ptr = 0;
    }

  }
  return -1;
}

/* ------------------------------------------------------------------------
 * reset the frame buffer to "empty"
 * --------------------------------------------------------------------- */
void framebuf_reset() {
  buffer.head = 0;
  buffer.tail = 0;
}

/* ------------------------------------------------------------------------
 * how much data is currently in the buffer 
 * --------------------------------------------------------------------- */
int framebuf_size() {
  if(buffer.head == buffer.tail) {
    // empty or full
    return 0;
  }

  if(buffer.head < buffer.tail) {
    return buffer.tail - buffer.head;
  } else {
    return FRAMEBUFLEN - (buffer.head - buffer.tail);
  }
}

/* ------------------------------------------------------------------------
 * put a single byte of data into the frame buffer
 * --------------------------------------------------------------------- */
int framebuf_put(uint8_t data) {
  // we write to tail and then advance the tail ....
  buffer.data[buffer.tail] = data;
  buffer.tail++;

  // if the tail has gone beyond the size it becomes 0
  if (buffer.tail >= FRAMEBUFLEN) {
    buffer.tail = 0;
  }
  return 1;
}

/* ------------------------------------------------------------------------
 * write an array of bytes to the frame buffer
 * --------------------------------------------------------------------- */
int framebuf_write(uint8_t *data, size_t len) {
  for(int i=0;i<len;i++) {
    framebuf_put(data[i]);
  }
  return 1;
}

/* ------------------------------------------------------------------------
 * return the byte size of the next ready frame or -1 if no frame is ready.
 * --------------------------------------------------------------------- */
int framebuf_frame_size() {
  int start = index(SFLAG);
  if (start == -1) {
    return 0;
  }

  int end = index(EFLAG);
  if (end == -1) {
    return 0;
  }

  int framesize;

  assert(start != end);

  if (start < end) {
    return end - start + 1;
  }
  
  return FRAMEBUFLEN - (start - end) + 1;
}


/* ------------------------------------------------------------------------
 * deframe len bytes from the buffer. len needs to be the correct size of 
 * the next frame. it can be found by calling framebuf_frame_size().
 * data (the result) needs to be big enough to hold len bytes.
 * 
 * Control characters are removed from the bytes. I.e. data is unescaped.
 * --------------------------------------------------------------------- */
int framebuf_deframe(uint8_t *data, size_t len) {
  int srcptr = buffer.head;
  int destptr = 0;
  int escape = 0;

  for(int i=0;i<len;i++) {

    // do we have the escape char and are not in escape
    if (buffer.data[srcptr] == ESCAPE && escape == 0) {
      // skip writing the control character to dest and do
      // not increment the dest pointer
      escape = 1;
      srcptr++;
    } else {
      // write src to dest and increment both pointers
      data[destptr] = buffer.data[srcptr];
      srcptr++;
      destptr++;
      escape = 0;      
    }
    
    // if ptr has reached the end of the array set it to the beginning
    if (srcptr >= FRAMEBUFLEN) {
      srcptr = 0;
    }              
  }

  // head is now ptr
  buffer.head = srcptr ;

  return 1;
}



/* ========================================================================
 * ============================ FRAMELST  =================================
 * ======================================================================== */


// Create a framelst
framelst_t *framelst_create() {
  return 0x0;
}

// Free a frame list. This does not free any memory associated with
// frames. It only frees the list memory
void framelst_free(framelst_t *lst) {
}

// Add a frame to the frame list
void framelst_add(framelst_t *lst, frame_t *frame) {
}

// Remove a frame from a framelst. This does NOT free the frame memory.
int framelst_remove(framelst_t *lst, frame_t *frame) {
  return 0;
}

// Find a frame by id in a framelst
frame_t *framelst_find_frame_by_id(framelst_t *lst, uint8_t id) {
  return 0x0;
}

// Find a frame by refid in a framelst
frame_t *framelst_find_frame_by_refid(framelst_t *lst, uint8_t refid) {
  return 0x0;
}
