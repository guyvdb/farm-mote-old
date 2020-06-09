#include "frame.h"
#include "bytes.h"
#include "../console/log.h"

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>

static framebuf_t buffer; // the one and only framebuf circular buffer 
static uint16_t nextid = 0;


/* ========================================================================
 * ============================== FRAME  ==================================
 * ======================================================================== */

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
uint16_t frame_next_id(void) {
  nextid ++;

  if (nextid==0) {
    nextid = 1;
  }
  return nextid;
}

void frame_args_begin(frame_t *frame) {
  frame->argptr = 0;
}

void frame_args_end(frame_t *frame) {
  frame->argptr = 0;
}

/* ------------------------------------------------------------------------
 * Allocate a frame structure. Also allocate space for the payload, if any.
 * The caller must free the memory.
 * --------------------------------------------------------------------- */
frame_t *frame_create(uint8_t cmd, int payloadlen) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  frame_t *frame = malloc(sizeof(frame_t));
  frame->len = (uint8_t)payloadlen;
  frame->next = 0x0;
  frame->prev = 0x0;
  frame->argptr = 0x0;
  frame->version = FRAMEVERSION;
  frame->id = frame_next_id();
  frame->refid = 0x0;
  frame->cmd = cmd;
  frame->tcount = 1;
  frame->transmitted = (int32_t)tv.tv_sec;


  if(frame->len > 0) {
    frame->payload = (uint8_t*)malloc((int)frame->len);
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
  frame->argptr = 0x0;

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
 * return the unescaped frame bytes. the caller needs to free the buffer 
 * --------------------------------------------------------------------- */
uint8_t *frame_encode_frame_bytes(frame_t *frame, int *len) {

  // calculate how many frame byte we are going to use
  int flen = (int)FRAMELENWITHOUTPAYLOAD + (int)frame->len;
  uint8_t *buf = (uint8_t*)malloc(flen);
  uint8_t *ptr = buf;
  
  *len = flen;
  
  bytes_uint8_encode(frame->version, ptr); ptr += 1;
  bytes_uint16_encode(frame->id, ptr); ptr += 2;
  bytes_uint8_encode(frame->tcount, ptr); ptr += 1;
  bytes_uint32_encode(frame->transmitted, ptr); ptr += 4;
  bytes_uint16_encode(frame->refid, ptr); ptr += 2;
  bytes_uint8_encode(frame->cmd, ptr); ptr += 1;
  bytes_uint8_encode(frame->len, ptr); ptr += 1;

  for(int i=0; i< (int)frame->len; i++) {
    ptr[i] = frame->payload[i];
  }

  return buf;
}

/* ------------------------------------------------------------------------
 * return the escaped network bytes including SFLAG & EFLAG. the caller 
 * needs to free the buffer 
 * --------------------------------------------------------------------- */
uint8_t *frame_encode_network_bytes(frame_t *frame, int *len) {
  uint8_t *fbuf;
  int fbuflen;
  uint8_t *nbuf;
  int nbuflen;

  int srcptr = 0;
  int destptr = 0;

  // alloc/get the frame bytes
  fbuf = frame_encode_frame_bytes(frame, &fbuflen);

  // count the number of control characters
  nbuflen = fbuflen + 2; // +2 for SFLAG & EFLAG  

  // count the number of control characters in the frame bytes 
  for(int i=0;i<fbuflen;i++) {
    if( (fbuf[i] == SFLAG) || (fbuf[i] == EFLAG) || (fbuf[i] == ESCAPE) ){
      nbuflen++;
    }
  }

  // alloc the network bytes
  nbuf = (uint8_t*)malloc(nbuflen);

  // write the SFLAG 
  nbuf[destptr] = SFLAG; destptr++;
  
  // copy the network bytes
  for(int i=0;i<fbuflen;i++) {

    // if the char is a control character add an escape before the data
    // else just copy the byte across
    if( (fbuf[i] == SFLAG) || (fbuf[i] == EFLAG) || (fbuf[i] == ESCAPE) ){
      nbuf[destptr] = ESCAPE;
      destptr++;
      nbuf[destptr] = fbuf[srcptr];
      destptr++;
      srcptr++;
    } else {
      nbuf[destptr] = fbuf[srcptr];
      destptr++;
      srcptr++;
    }
  }

  // write the EFLAG
  nbuf[destptr] = EFLAG; destptr++;

  // free the frame bytes that we allocated 
  free(fbuf);

  // set the result length of the network bytes buffer 
  *len = nbuflen;

  // return the network bytes
  return nbuf;
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
 * If we convert this int to a char * how big does the buffer need to be 
 * --------------------------------------------------------------------- */
/*static int itoa_buf_size(int value) {
  // 0 - 9
  // 10 -- 99
  // 100 - 999
  // 1000 - 9999
  // 10000 - 99999
  
  }*/

/*
ssize_t bufsz = snprintf(NULL, 0, "{data:%d}",12312);
char* buf = malloc(bufsz + 1);
snprintf(buf, bufsz + 1, "{data:%d}",12312);

...

free(buf);
*/

/* ------------------------------------------------------------------------
 * Create a string representation of this frame 
 * --------------------------------------------------------------------- */
/*
char *frame_to_string(frame_t *frame) {
  int len = (int)frame->len;
  
                                           
  size_t slen = snprintf(0x0,0,"&{%d %d %d %d %d %d %d [");
  size_t elen = snprintf(0x0,0,"] %d}");
  size_t plen = 0;

  for (int i=0;i<len;i++) {
    plen += snprintf(0x0,0,"%d ", frame->payload[i]);
  }
  

  char *result = malloc(slen+elen+plen);


  

  
  
  
  

  printf("&{%d %d %d %d %d %d %d [");
  printf);


  
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
*/

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_get_arg_uint8(frame_t *frame, uint8_t *result){
  uint8_t *ptr = frame->payload; 
  ptr += frame->argptr;
  
  if(1 + frame->argptr <= frame->len){
    *result = bytes_uint8_decode(ptr);
    frame->argptr += 1;
    return 1;
  }
  return 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_get_arg_uint16(frame_t *frame, uint16_t *result){
  uint8_t *ptr = frame->payload;
  ptr += frame->argptr;
  
  if(2 + frame->argptr <= frame->len){
    *result = bytes_uint16_decode(ptr);
    frame->argptr += 2;
    return 1;
  }
  return 0;  
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_get_arg_uint32(frame_t *frame, uint32_t *result){
  uint8_t *ptr = frame->payload;
  ptr += frame->argptr;
  
  if(4 + frame->argptr <= frame->len){
    *result = bytes_uint32_decode(ptr);
    frame->argptr += 4;
    return 1;
  }
  return 0;  
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */

int frame_put_arg_uint8(frame_t *frame, uint8_t value){
  uint8_t *ptr = frame->payload;
  ptr += frame->argptr;
  
  if(1 + frame->argptr <= frame->len) {
    bytes_uint8_encode(value, ptr);
    frame->argptr += 1;
    return 1;
  }
  return 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_put_arg_uint16(frame_t *frame, uint16_t value){
  uint8_t *ptr = frame->payload; 
  ptr += frame->argptr;
  
  if(2 + frame->argptr <= frame->len) {
    bytes_uint16_encode(value, ptr);
    frame->argptr += 2;
    return 1;
  }
  return 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_put_arg_uint32(frame_t *frame, uint32_t value){
  uint8_t *ptr = frame->payload;
  ptr += frame->argptr;
  
  if(4 + frame->argptr <= frame->len) {
    bytes_uint32_encode(value, ptr);
    frame->argptr += 4;
    return 1;
  }
  return 0;
}




/* ========================================================================
 * ============================ FRAMEBUF  =================================
 * ======================================================================== */



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
  buffer.in = 0;
  buffer.out = 0;
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
  buffer.in++;

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
  int start = byteindex(SFLAG);
  if (start == -1) {
    return 0;
  }

  int end = byteindex(EFLAG);
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
      buffer.out++;
    } else {
      // write src to dest and increment both pointers
      data[destptr] = buffer.data[srcptr];
      srcptr++;
      destptr++;
      buffer.out++;
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




// Analyse the frame buffer
void framebuf_debug(void) {
  printf("=== FRAME BUFFE ===\n");
  printf(" head: %d\n", buffer.head);
  printf(" tail: %d\n", buffer.tail);
  printf(" in: %d\n", buffer.in);
  printf(" out: %d\n", buffer.out);
  printf(" delta: %d\n", buffer.in - buffer.out);
  printf(" size: %d\n", framebuf_size());
  printf(" frame size: %d\n", framebuf_frame_size());
  printf("=== END ===\n\n");
  
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
