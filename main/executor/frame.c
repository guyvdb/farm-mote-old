#include "frame.h"
#include "bytes.h"


#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>


static framebuf_t buffer; // the one and only framebuf circular buffer 


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
frame_t *frame_create(uint8_t *payload, uint8_t len) {
  frame_t *frame = malloc(sizeof(frame_t));
  frame->len = len;
  
  
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
 * 
 * --------------------------------------------------------------------- */
frame_t *frame_from_bytes(uint8_t *data, size_t len) {
  int flen;

  uint8_t *ptr = data;

  
  // alloc frame memory 
  frame_t *frame = malloc(sizeof(frame_t));

  

  // skip SFLAG - increment 1 byte
  //printf("sflag byte[0] = %d\n", ptr[0]);
  ptr++;

  

  // version
  //printf("version byte[0] = %d\n", ptr[0]);
  frame->version = bytes_uint8_decode(ptr); ptr += 1;


  
  // id
  //printf("id byte[0] = %d, byte[1] = %d\n", ptr[0], ptr[1]);
  frame->id =  bytes_uint16_decode(ptr);  ptr += 2;


  // tcount
  //printf("tcount byte[0] = %d\n", ptr[0]);
  frame->tcount = bytes_uint8_decode(ptr); ptr += 1;

  // transmitted timestamp
  //printf("transmitted byte[0] = %d, byte[1] = %d, byte[2] = %d, byte[3] = %d\n", ptr[0], ptr[1], ptr[2], ptr[3]);
  frame->transmitted = (int32_t)bytes_uint32_decode(ptr); ptr += 4;

  // ref id
  //printf("ref id byte[0] = %d, byte[1] = %d\n", ptr[0], ptr[1]);
  frame->refid = bytes_uint16_decode(ptr); ptr += 2;

  
  // cmd
  //printf("cmd byte[0] = %d\n", ptr[0]);
  frame->cmd = bytes_uint8_decode(ptr); ptr += 1;

  // len
  //printf("len byte[0] = %d\n", ptr[0]);
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
 * 
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




// CLEAN UP BELOW -----------------------------------------











/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// find the index of first occurence of nonescaped data
// starting from head and proceeding to tail. return index or -1
int index(uint8_t data) {
  size_t size = framebuf_size();
  int ptr = buffer.head;
  
  for(int i=0;i<size;i++) {
    if(buffer.data[ptr] == ESCAPE) {
      // skip the next data element even if it matches
      printf("WARNING: escape is not implemented yet.\n");
    } else {
      if(buffer.data[ptr] == data) {
        return ptr;
      }
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


///// Public

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void framebuf_reset() {
  buffer.head = 0;
  buffer.tail = 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// how much data is currently in the buffer 
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
 * 
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
 * 
 * --------------------------------------------------------------------- */
int framebuf_write(uint8_t *data, size_t len) {
  for(int i=0;i<len;i++) {
    framebuf_put(data[i]);
  }
  return 1;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// return the size of the next ready frame or -1
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
 * 
 * --------------------------------------------------------------------- */
// deframe len bytes from the buffer. data needs to
// be big enough to hold len bytes
int framebuf_deframe(uint8_t *data, size_t len) {
  int ptr = buffer.head;

  for(int i=0;i<len;i++) {
    data[i] = buffer.data[ptr];

    // increment the ptr 
    ptr++;

    
    // if ptr has reached the end of the array set it to the beginning
    if (ptr >= FRAMEBUFLEN) {
      ptr = 0;
    }              
  }

  // head is now ptr
  buffer.head = ptr ;

  return 1;

}

