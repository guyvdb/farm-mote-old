#include "framebuf.h"
#include <assert.h>
#include <stdio.h>

// Local variables 
static framebuf_t buffer; // the one and only framebuf circular buffer 


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
/*
static void print_bytes(const char *msg, uint8_t *data, size_t len) {
  printf("%s [", msg);
  for(int i=0;i<len;i++) {
    if(i+1 == len) {
      printf("%d", data[i]);
    } else {
      printf("%d ", data[i]);
    }
  }
  printf("] len=%d\n", len);
  }*/

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
/*static void print_framebuf(void) {
  int idx = buffer.head;

  printf("framebuf: [");
  for(;;) {
    printf("%d ", buffer.data[idx]);

    idx++;

    if (idx >= FRAMEBUF_DATA_LEN) {
      idx = 0;
    }

    if(idx == buffer.tail) {
      break;
    }        
  }
  printf("]\n");
  
  }*/

/* ------------------------------------------------------------------------
 * find the index in the circular buffer of first occurence of nonescaped 
 * data starting from head and proceeding to tail. return index or -1
 * --------------------------------------------------------------------- */
static int byteindex(uint8_t data) {
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
    if (ptr >= FRAMEBUF_DATA_LEN) {
      ptr = 0;
    }

  }
  return -1;
}

/* ------------------------------------------------------------------------
 * return the byte size of the next ready frame or -1 if no frame is ready.
 * --------------------------------------------------------------------- */
static size_t frame_size() {
  int start = byteindex(SFLAG);
  if (start == -1) {
    return 0;
  }

  //printf("framebuf index(SFLAG) = %d\n", start);

  int end = byteindex(EFLAG);
  if (end == -1) {
    return 0;
  }

  //printf("framebuf index(EFLAG) = %d\n", end);

  

  int framesize;

  assert(start != end);

  if (start < end) {
    return end - start + 1;
  }
  
  return FRAMEBUF_DATA_LEN - (start - end) + 1;
}

/* ------------------------------------------------------------------------
 * deframe len bytes from the buffer. len needs to be the correct size of 
 * the next frame. it can be found by calling frame_size(). Do not return 
 * the SFLAG & EFLAG and unescape the data before returning it.
 * The amount of data returned will be smaller than len bytes.
 * data - the byte buffer to store the data in 
 * len - the number of bytes of the unescaped frame. also min size of data
 * return - 0 on error or the number of bytes written to data.
 * --------------------------------------------------------------------- */
static int extract_frame_bytes(uint8_t *data, size_t len) {
  int srcptr = buffer.head;
  int destptr = 0;
  int escape = 0;

  //printf("extact_frame_bytes() len = %d\n", len);
  //print_framebuf();

  for(int i=0;i<len;i++) {
    // do we have the escape char and are not in escape
    if (buffer.data[srcptr] == ESCAPE && escape == 0) {
      // skip writing the control character to dest and do
      // not increment the dest pointer
      escape = 1;
      srcptr++;
    } else {
      // is the char SFLAG or EFLAG and we are not escapes
      // if so it is the first or last char and we should
      // drop it 
      if( (buffer.data[srcptr] == SFLAG || buffer.data[srcptr] == EFLAG) && escape == 0 ){
        // drop the char
        srcptr++;
      } else {
        // write src to dest and increment both pointers
        data[destptr] = buffer.data[srcptr];
        srcptr++;
        destptr++;
        escape = 0; 
      }
    }
    
    // if ptr has reached the end of the array set it to the beginning
    if (srcptr >= FRAMEBUF_DATA_LEN) {
      srcptr = 0;
    }              
  }

  // head is now ptr
  buffer.head = srcptr ;

  return destptr;
}

/* ------------------------------------------------------------------------
 * Reset the head and tail of the framebuf to 0. This zeros the buffer.
 * All data that was in the buffer is lost.
 * --------------------------------------------------------------------- */
void framebuf_reset(void) {
  buffer.head = 0;
  buffer.tail = 0;
}

/* ------------------------------------------------------------------------
 * How much data is currently in the framebuf. What
 * is the total byte count.
 * return - the number of bytes in use.
 * --------------------------------------------------------------------- */
int framebuf_size() {
  if(buffer.head == buffer.tail) {
    // empty or full
    return 0;
  }

  if(buffer.head < buffer.tail) {
    return buffer.tail - buffer.head;
  } else {
    return FRAMEBUF_DATA_LEN - (buffer.head - buffer.tail);
  }
}

/* ------------------------------------------------------------------------
 * Put a single byte into the framebuf.
 * data - the byte to put in the buffer
 * return - 1 on success or 0 on failure 
 * --------------------------------------------------------------------- */
int framebuf_put(uint8_t data) {
  // we write to tail and then advance the tail ....
  buffer.data[buffer.tail] = data;
  buffer.tail++;

  // if the tail has gone beyond the size it becomes 0
  if (buffer.tail >= FRAMEBUF_DATA_LEN) {
    buffer.tail = 0;
  }
  return 1;
}

/* ------------------------------------------------------------------------
 * Write an array of bytes to the framebuf.
 * data - the array of bytes
 * len - the size of the array
 * return - 1 on success or 0 on failure
 * --------------------------------------------------------------------- */
int framebuf_write(uint8_t *data, size_t len) {
  //print_bytes("framebuf_write()", data, len);
  for(int i=0;i<len;i++) {
    framebuf_put(data[i]);
  }
  return 1;
}

/* ------------------------------------------------------------------------
 * Deframe the next complete frame. The memory for the new
 * frame is allocated. The caller is responsiable for calling
 * frame_free() at some point.
 * return - a newly allocated frame or null if no frame exists
 * --------------------------------------------------------------------- */
frame_t *framebuf_deframe(void) {
  uint8_t buf[FRAME_STACK_BUF_LEN];
  size_t fsize = frame_size();
  if(fsize > 0) {
    if (fsize > FRAME_STACK_BUF_LEN) {
      printf("Out of memory error\n");
      return 0x0;
    }

    // static int extract_frame_bytes(uint8_t *data, size_t len) {
    int extracted = extract_frame_bytes(buf, fsize);

    //printf("fszize=%d, extracted=%d\n",fsize, extracted);
    //print_bytes("framebuf_deframe()->extract_frame_bytes()", buf,extracted);
    
    if (extracted > 0 ) {
      return frame_decode(buf, extracted);
    } else {
      return 0x0;
    }     
  } else {
    // there is no frame ready
    return 0x0;
  }
}

