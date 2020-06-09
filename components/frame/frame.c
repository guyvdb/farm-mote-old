#include "frame.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Local variables 
static uint16_t nextid = 0;


/* ------------------------------------------------------------------------
 * Return 1 if data is a control character otherwise return 0
 * --------------------------------------------------------------------- */
static int is_control_character(uint8_t data) {
  if(data == SFLAG || data == EFLAG || data == ESCAPE) {
    return 1;
  } else {
    return 0;
  }
}

/* ------------------------------------------------------------------------
 * Return a count of the number of control characters in the data buffer
 * --------------------------------------------------------------------- */
static size_t count_control_characters(uint8_t *data, size_t len) {
  size_t count = 0;
  for(int i=0;i<len;i++) {
    if(is_control_character(data[i])) {
      count++;
    }
  }
  return count;
}

/* ------------------------------------------------------------------------
 * Copy src to dest inserting control characters if needed. No bounds 
 * checking is done on dest. The caller should have performed a 
 * count_control_characters() and ensured that dest is big enought.
 * return the number of bytes encoded
 * --------------------------------------------------------------------- */
static int memcpy_escape(uint8_t *dest, uint8_t *src, size_t len) {
  size_t destptr = 0;
  for(int i=0; i< len;i++) {
    if(is_control_character(src[i])) {
      dest[destptr] = ESCAPE;
      destptr++;
    }
    dest[destptr] = src[i];
    destptr++;
  }
  return destptr;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int uint8_encode(uint8_t value, uint8_t *data, size_t len){  
  if(1 <= len) {
    data[0] = value;
    return 1;
  } else {
    return 0;
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int uint16_encode(uint16_t value, uint8_t *data, size_t len){
  if(2 <= len) {
    data[0] = value >> 8;
    data[1] = value;
    return 1;
  } else {
    return 0;
  }  
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int uint32_encode(uint32_t value, uint8_t *data, size_t len){
  if(1 <= len) {
    data[0] = value >> 24;
    data[1] = value >> 16;
    data[2] = value >> 8;
    data[3] = value;    
    return 1;
  } else {
    return 0;
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int string_encode(const char *value, uint8_t *data, size_t len) {
  size_t slen = strlen(value);
  if (slen + 1 <= len) {
    data[0] = (uint8_t)slen;
    for(int i=0;i<slen;i++) {
      data[i+1] = (uint8_t)value[i];
    }
    return 1;
  } else {
    return 0;
  }
}



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int uint8_encode_len(uint8_t value) {
  if(value == SFLAG || value == EFLAG || value == ESCAPE) {
    return 2;
  } else {
    return 1;
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int uint16_encode_len(uint16_t value) {
  uint8_t buf[2];
  int len = 2;
  uint16_encode(value, buf,2);

  for(int i=0;i<2;i++) {
    if(buf[i] == SFLAG || buf[i] == EFLAG || buf[i] == ESCAPE) {
      len ++;
    }
  }
  return len;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int uint32_encode_len(uint32_t value) {
  uint8_t buf[4];
  int len = 4;
  uint32_encode(value, buf,4);

  for(int i=0;i<4;i++) {
    if(buf[i] == SFLAG || buf[i] == EFLAG || buf[i] == ESCAPE) {
      len ++;
    }
  }
  return len;  
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int payload_encode_len(frame_t *frame) {
  int len = (int)frame->len;

  for(int i=0;i<(int)frame->len;i++) {
    if(frame->payload[i] == SFLAG || frame->payload[i] == EFLAG || frame->payload[i] == ESCAPE) {
      len++;
    }
  }
  return len;
}

/* ------------------------------------------------------------------------
 * Decode a uint8 value from data. Store the result in value. 
 * value - an address to accept the value 
 * data - a byte array of data 
 * len - the size of the byte array
 * result - return 1 on success or 0 on failure - not enough data to decode
 * --------------------------------------------------------------------- */
static int uint8_decode(uint8_t *value, uint8_t *data, size_t len) {
  if(len < 1) {
    // out of memory to read
    return 0;
  } else {
    *value = data[0];
    return 1;
  }
}

/* ------------------------------------------------------------------------
 * Decode a uint16 value from data. Store the result in value. 
 * value - an address to accept the value 
 * data - a byte array of data 
 * len - the size of the byte array
 * result - return 1 on success or 0 on failure - not enough data to decode
 * --------------------------------------------------------------------- */
static int uint16_decode(uint16_t *value, uint8_t *data, size_t len) {
  if(len < 2) {
    // out of memory to read
    return 0;
  } else {
    *value = (data[0] <<8) + data[1];
    return 1;
  }  
}

/* ------------------------------------------------------------------------
 * Decode a uint32 value from data. Store the result in value. 
 * value - an address to accept the value 
 * data - a byte array of data 
 * len - the size of the byte array
 * result - return 1 on success or 0 on failure - not enough data to decode
 * --------------------------------------------------------------------- */
static int uint32_decode(uint32_t *value, uint8_t *data, size_t len) {
  if(len < 4) {
    // out of memory to read
    return 0;
  } else {
    *value = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
    return 1;
  }    
}

/* ------------------------------------------------------------------------
 * Decode a null terminated string  value from data. Allocate memory and 
 * store the result in value. The caller is responsiable to call free()
 * value - an address to allocate mem and store the value 
 * data - a byte array of data 
 * len - the size of the byte array
 * result - return 1 on success or 0 on failure - not enough data to decode
 * --------------------------------------------------------------------- */
static int string_decode(char *value, uint8_t *data, size_t len) {
  size_t slen;
  // we are going to alloc memory at value

  if(len < 1) {
    // out of memory to read
    return 0;
  } else {
    slen = (size_t)data[0];
  }

  // need to add 1 byte for null terminator
}


  
/* ------------------------------------------------------------------------
 * Encode a uint8 value for transmission on the network. If needed escape
 * any bytes as needed. The data buffer needs up to twice the size of the
 * value representation if every byte needs to be escaped.
 * value - the value to encode 
 * data - the data buffer to write the value to 
 * len - the size of the data buffer 
 * return - the number of bytes written on success or 0 on failure 
 * --------------------------------------------------------------------- */
static int uint8_encode_escape(uint8_t value, uint8_t *data, size_t len) {
  uint8_t buf[1];
  size_t reqlen;

  // do the encoding 
  buf[0] = value;


  // do the escaping 
  reqlen = count_control_characters(buf, 1) + 1; // +1 for uint8 value
  if (reqlen < len) {
    return memcpy_escape(data, buf, 1);
  } else {
    // not enough memory
    return 0;
  }
}

/* ------------------------------------------------------------------------
 * Encode a uint16 value for transmission on the network. If needed escape
 * any bytes as needed. The data buffer needs up to twice the size of the
 * value representation if every byte needs to be escaped.
 * value - the value to encode 
 * data - the data buffer to write the value to 
 * len - the size of the data buffer 
 * return - the number of bytes written on success or 0 on failure 
 * --------------------------------------------------------------------- */
static int uint16_encode_escape(uint16_t value, uint8_t *data, size_t len) {
  uint8_t buf[2];
  size_t reqlen;

  // do the encoding 
  buf[0] = value >> 8;
  buf[1] = value;
  

  // do the escaping 
  reqlen = count_control_characters(buf, 2) + 2; // +2 for uint16 value
  if (reqlen < len) {
    return memcpy_escape(data, buf, 2);
  } else {
    // not enough memory
    return 0;
  }  
}

/* ------------------------------------------------------------------------
 * Encode a uint32 value for transmission on the network. If needed escape
 * any bytes as needed. The data buffer needs up to twice the size of the
 * value representation if every byte needs to be escaped.
 * value - the value to encode 
 * data - the data buffer to write the value to 
 * len - the size of the data buffer 
 * return - the number of bytes written on success or 0 on failure 
 * --------------------------------------------------------------------- */
static int uint32_encode_escape(uint32_t value, uint8_t *data, size_t len) {
  uint8_t buf[4];
  size_t reqlen;

  // do the encoding 
  buf[0] = value >> 24;
  buf[1] = value >> 16;
  buf[2] = value >> 8;
  buf[3] = value;

  // do the escaping 
  reqlen = count_control_characters(buf, 4) + 4; // +4 for uint32 value
  if (reqlen < len) {
    return memcpy_escape(data, buf, 4);
  } else {
    // not enough memory
    return 0;
  }    
}

/* ------------------------------------------------------------------------
 * Encode a null terminated string  value for transmission on the network. 
 * If needed escape any bytes as needed. The data buffer needs up to twice 
 * the size of the value representation if every byte needs to be escaped.
 * value - the value to encode - must be null terminated string 
 * data - the data buffer to write the value to 
 * len - the size of the data buffer 
 * return - the number of bytes written on success or 0 on failure 
 * --------------------------------------------------------------------- */
static int string_encode_escape(char *value, uint8_t *data, size_t len) {
  uint8_t buf[256];
  size_t reqlen;
  size_t slen;
  char *v = value;

  // Get the lenght of the string and write it to the front of the buffer
  // as uint8.
  slen = strlen(value);
  buf[0] = (uint8_t)slen;
  

  // do the encoding
  //copy the string without null termination
  size_t ptr = 1; // position 0 is the string len
  while(*v != 0x0) {
    buf[ptr] = *v;
    ptr++;
    if (ptr > 255)  {
      // out of memory
      return 0;
    }
  }
 
  // do the escaping 
  reqlen = count_control_characters(buf, slen+1) + slen + 1; // + slen + 1 for string lenght and len encoding
  if (reqlen < len) {
    return memcpy_escape(data, buf, slen + 1);
  } else {
    // not enough memory
    return 0;
  }  
}


/* ------------------------------------------------------------------------
 * get the next frame id.  sequence is reset to 1 at reboot.
 * return - an auto incrementing integer sequence. 
 * --------------------------------------------------------------------- */
uint16_t frame_next_id(void) {
  nextid ++;

  if (nextid==0) {
    nextid = 1;
  }
  return nextid;
}

/* ------------------------------------------------------------------------
 * Create a frame. Memory is allocated for the frame and len bytes is allocated for the payload
 * cmd - the command to set on the frame
 * ref - the refid of another frame
 * timestamp - the timestamp to place on the frame 
 * len - the length of the payload buffer allocated inside the frame
 * return - a pointer to a new frame or null on error.
 * --------------------------------------------------------------------- */
frame_t *frame_create(uint8_t cmd, uint8_t ref, int32_t timestamp, size_t len) {
  frame_t *frame = malloc(sizeof(frame_t));
  frame->len = (uint8_t)len;
  frame->argptr = 0x0;
  frame->version = FRAME_PROTOCOL_VERSION;
  frame->id = frame_next_id();
  frame->refid = ref;
  frame->cmd = cmd;
  frame->tcount = 1;
  frame->timestamp = timestamp;

  if(len > 0) {
    frame->payload = (uint8_t*)malloc(len);
  } else {
    frame->payload = 0x0;
  }
  return frame;
}


/* ------------------------------------------------------------------------
 * Create a frame from a set of bytes that have been pulled off the wire.  Memory is allocated
 * for the frame_t struct and the frames internal payload. A call to frame_free() needs to
 * be performed by the caller to free the memory. The data being passed in is unescaped with
 * no control characters present.
 * data - the bytes that have been pulled of the wire and unescaped
 * len - the length of the data
 * return - a new frame pointer or null if an error occured
 * --------------------------------------------------------------------- */
frame_t *frame_decode(uint8_t *data, size_t len) {
  int flen;
  uint8_t *ptr = data;
  uint32_t timestamp;

  // alloc frame memory 
  frame_t *frame = malloc(sizeof(frame_t));
  frame->argptr = 0x0;

  // version
  uint8_decode(&frame->version, ptr, 1); ptr += 1;                                          
    
  // id
  uint16_decode(&frame->id, ptr, 2); ptr += 2;
  
  // tcount
  uint8_decode(&frame->tcount, ptr, 1); ptr += 1;

  // transmitted timestamp
  uint32_decode(&timestamp, ptr, 4); ptr += 4;
  frame->timestamp = (uint32_t) timestamp;
  
  // ref id
  uint16_decode(&frame->refid, ptr, 2); ptr += 2;
  
  // cmd
  uint8_decode(&frame->cmd, ptr, 1); ptr += 1;
  
  // len
  uint8_decode(&frame->len, ptr, 1); ptr += 1;
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
 * Given a frame, encode it into a series of bytes that can be sent across the network.
 * Include the SFLAG & EFLAG markers and ensure that the data is escaped.
 * frame  - the frame to encode.
 * data   - the buffer to place the encoded bytes into.
 * len    - the size of the buffer.
 * return - bytes encoded on success or 0 on failure 
 * --------------------------------------------------------------------- */
int frame_encode(frame_t *frame, uint8_t *data, size_t len) {
  uint8_t *ptr = data;
  size_t used = 0;
  int writen;

  // SFLAG
  ptr[0] = SFLAG;
  ptr++;
  used++;
  

  // version 
  writen = uint8_encode_escape(frame->version, ptr, len - used);
  if(writen == 0) return 0;
  used += writen;
  ptr += writen;


  // Id
  writen = uint16_encode_escape(frame->id, ptr, len - used);
  if(writen == 0) return 0;
  used += writen;
  ptr += writen;

  // TCount
  writen = uint8_encode_escape(frame->tcount, ptr, len - used);
  if(writen == 0) return 0;
  used += writen;
  ptr += writen;


  // Timestamp
  writen = uint32_encode_escape(frame->timestamp, ptr, len - used);
  if(writen == 0) return 0;
  used += writen;
  ptr += writen;

  
  // RefId
  //printf("write ref id: %d\n", frame->refid);
  writen = uint16_encode_escape(frame->refid, ptr, len - used);
  if(writen == 0) return 0;

  //printf("wrote %d bytes\n", writen);

  //printf("bytes are: %d %d %d\n", ptr[0], ptr[1], ptr[2]);

  
  used += writen;  
  ptr += writen;


  // Cmd
  writen = uint8_encode_escape(frame->cmd, ptr, len - used);
  if(writen == 0) return 0;
  used += writen;  
  ptr += writen;
  

  // Len
  writen = uint8_encode_escape(frame->len, ptr, len - used);
  if(writen == 0) return 0;
  used += writen;  
  ptr += writen;

  // Payload
  for(int i=0; i<(int)frame->len; i++) {
    if(frame->payload[i] == SFLAG || frame->payload[i] == EFLAG || frame->payload[i] == ESCAPE) {
      *ptr = ESCAPE;
      ptr++;
      used++;

      *ptr = frame->payload[i];
      ptr++;
      used++;
      
    } else {
      *ptr = frame->payload[i];
      ptr++;
      used++;
    }

    if(len-used <= 1) {
      return 0; 
    }
  }

  *ptr = EFLAG;
  used++;


  return used;  
}

/* ------------------------------------------------------------------------
 * Given a frame, calculate the number of bytes that this frame will encode to.
 * The len must include any control characters that will be added.
 * frame - the frame to do the calculation on.
 * return - the number of bytes needed to encode this frame.
 * --------------------------------------------------------------------- */
int frame_encoded_len(frame_t *frame) {
  int len = 2;  // Space for SFLAG and EFLAG

  len += uint8_encode_len(frame->version);
  len += uint16_encode_len(frame->id);
  len += uint8_encode_len(frame->tcount);
  len += uint32_encode_len((uint32_t)frame->timestamp);
  len += uint16_encode_len(frame->refid);
  len += uint8_encode_len(frame->cmd);
  len += uint8_encode_len(frame->len);
  len += payload_encode_len(frame);

  return len;
 
}



/* ------------------------------------------------------------------------
 * return the unescaped frame bytes. the caller needs to free the buffer 
 * --------------------------------------------------------------------- */
/*uint8_t *frame_encode_frame_bytes(frame_t *frame, int *len) {

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
  }*/

/* ------------------------------------------------------------------------
 * return the escaped network bytes including SFLAG & EFLAG. the caller 
 * needs to free the buffer 
 * --------------------------------------------------------------------- */
/*uint8_t *frame_encode_network_bytes(frame_t *frame, int *len) {
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
  }*/

/* ------------------------------------------------------------------------
 * Free a frame. Free the frame struct memory and the memory that was allocated for *payload.
 * frame - the frame that should be freed
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
 * Create a string representation of this frame. This function alloc's 
 * mem and the caller needs to free it .
 * --------------------------------------------------------------------- */
char *frame_to_string(frame_t *frame) {
  int len = (int)frame->len;
                              
  size_t slen = snprintf(0x0,0,"{v: %d, id: %d, tc: %d, time: %d, ref: %d, cmd: %d, len: %d, args: [",
                         frame->version,
                         frame->id,
                         frame->tcount,
                         frame->timestamp,
                         frame->refid,
                         frame->cmd,
                         frame->len);
  
  size_t elen = snprintf(0x0,0,"] }");

  size_t plen = 0;

  for (int i=0;i<len;i++) {
    if (i+1 == len) {
      plen += snprintf(0x0,0,"%d", frame->payload[i]);
    } else {
      plen += snprintf(0x0,0,"%d ", frame->payload[i]);
    }
  }
  
  char *result = malloc(slen+elen+plen);
  char *ptr = result;

  slen = sprintf(ptr ,"{v: %d, id: %d, tc: %d, time: %d, ref: %d, cmd: %d, len: %d, args: [",
          frame->version,
          frame->id,
          frame->tcount,
          frame->timestamp,
          frame->refid,
          frame->cmd,
          frame->len);


  ptr += slen;

  for (int i=0;i<len;i++) {
    if(i+1==len) {
      plen = sprintf(ptr, "%d", frame->payload[i]);
    } else {
      plen = sprintf(ptr, "%d ", frame->payload[i]);
    }
    ptr += plen ;
  }

  sprintf(ptr,"] }");
  
  return result;
  

}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void frame_args_begin(frame_t *frame) {
  frame->argptr = 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void frame_args_end(frame_t *frame) {
  frame->argptr = 0;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_args_get_uint8(frame_t *frame, uint8_t *result){
  uint8_t *ptr = frame->payload; 
  ptr += frame->argptr;
  
  if(1 + frame->argptr < frame->len){ // +1 for uint8
    if(uint8_decode(result, ptr,1)) { 
      frame->argptr += 1;
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_args_get_uint16(frame_t *frame, uint16_t *result){
  uint8_t *ptr = frame->payload;
  ptr += frame->argptr;
  
  if(2 + frame->argptr < frame->len){  // +2 for uint16
    if(uint16_decode(result, ptr,2)) {
      frame->argptr += 2;
      return 1;
    } else {
      return 0;
    }
  }
  return 0;  
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_args_get_uint32(frame_t *frame, uint32_t *result){
  uint8_t *ptr = frame->payload;
  ptr += frame->argptr;
  
  if(4 + frame->argptr <= frame->len){  // +4 for uint32
    if(uint32_decode(result, ptr,4)) {
      frame->argptr += 4;
      return 1;
    } else {
      return 0;
    }
  }
  return 0;  
}

/* ------------------------------------------------------------------------
 * Memory is allocated to result and the caller needs to free it 
 * --------------------------------------------------------------------- */
int frame_args_get_string(frame_t *frame, char *result) {
  return 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */

int frame_args_put_uint8(frame_t *frame, uint8_t value){
  uint8_t *ptr = frame->payload;
  ptr += frame->argptr;
  
  if(1 + frame->argptr < frame->len) {
    uint8_encode(value, ptr,1);
    frame->argptr += 1;
    return 1;
  }
  return 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_args_put_uint16(frame_t *frame, uint16_t value){
  uint8_t *ptr = frame->payload; 
  ptr += frame->argptr;
  
  if(2 + frame->argptr < frame->len) {
    uint16_encode(value, ptr,2);
    frame->argptr += 2;
    return 1;
  }
  return 0;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_args_put_uint32(frame_t *frame, uint32_t value){
  uint8_t *ptr = frame->payload;
  ptr += frame->argptr;
  
  if(4 + frame->argptr < frame->len) {
    uint32_encode(value, ptr,4);
    frame->argptr += 4;
    return 1;
  }
  return 0;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
int frame_args_put_string(frame_t *frame, const char* value) {
  uint8_t *ptr = frame->payload;
  uint8_t len = (uint8_t)strlen(value);
  ptr += frame->argptr;
  //if(len + 1 < frame->len) {
    //    static int string_encode(const char *value, uint8_t *data, size_t len) {
      string_encode(value, ptr, frame->len - frame->argptr);
      //}

  return 1;
}
