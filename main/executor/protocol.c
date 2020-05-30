

#include "protocol.h"


#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>




struct protobuf {
  uint8_t data[PROTOBUFLEN];
  size_t head;
  size_t tail;
};

typedef struct protobuf protobuf_t;


static protobuf_t buffer;



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// find the index of first occurence of nonescaped data
// starting from head and proceeding to tail. return index or -1
int index(uint8_t data) {
  size_t size = protobuf_size();
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
    if (ptr >= PROTOBUFLEN) {
      ptr = 0;
    }

  }
  return -1;
}


///// Public

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void protobuf_reset() {
  buffer.head = 0;
  buffer.tail = 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// how much data is currently in the buffer 
int protobuf_size() {
  if(buffer.head == buffer.tail) {
    // empty or full
    return 0;
  }

  if(buffer.head < buffer.tail) {
    return buffer.tail - buffer.head;
  } else {
    return PROTOBUFLEN - (buffer.head - buffer.tail);
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int protobuf_put(uint8_t data) {
  // we write to tail and then advance the tail ....
  buffer.data[buffer.tail] = data;
  buffer.tail++;

  // if the tail has gone beyond the size it becomes 0
  if (buffer.tail >= PROTOBUFLEN) {
    buffer.tail = 0;
  }

  return 1;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int protobuf_write(uint8_t *data, size_t len) {
  for(int i=0;i<len;i++) {
    protobuf_put(data[i]);
  }
  return 1;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// return the size of the next ready frame or -1
int protobuf_frame_size() {
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
    printf("frame size(1): start < end : %d : start = %d, end = %d\n", end - start + 1, start, end);
    return end - start + 1;
  }

  printf("frame size(2): end < start :  %d : start = %d, end = %d\n", PROTOBUFLEN - (start - end) + 1, start, end );
 
  return PROTOBUFLEN - (start - end) + 1;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// deframe len bytes from the buffer. data needs to
// be big enough to hold len bytes
int protobuf_deframe(uint8_t *data, size_t len) {
  int ptr = buffer.head;

  for(int i=0;i<len;i++) {
    data[i] = buffer.data[ptr];

    // increment the ptr 
    ptr++;

    
    // if ptr has reached the end of the array set it to the beginning
    if (ptr >= PROTOBUFLEN) {
      ptr = 0;
    }              
  }

  // head is now ptr
  buffer.head = ptr ;

  return 1;

}

 


void protobuf_print() {
}
