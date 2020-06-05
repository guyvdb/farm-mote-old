#include "bytes.h"

#include <string.h>



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
uint8_t bytes_uint8_decode(uint8_t *data) {
  return data[0];
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
uint16_t bytes_uint16_decode(uint8_t *data) {
  return (data[0] << 8) + data[1];
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
uint32_t bytes_uint32_decode(uint8_t *data) {
  return (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
}


/* ------------------------------------------------------------------------
 * This allocates memory for result. The caller must free the memory.
 * The string is encoded into the payload as a uint8 len + len bytes that 
 * make up the string excluding any null termination. So we alloc len+1 
 * bytes, copy the data and null terminate the string.
 * --------------------------------------------------------------------- */
char *bytes_string_decode(uint8_t *data) {
  uint8_t *ptr = data;
  uint8_t len = bytes_uint8_decode(data);
  ptr++;
  
  char *result = malloc(((int)len)+1);
  for(int i=0; i<(int)len; i++) {
    result[i] = ptr[i];
  }
  result[(int)len] = 0x0;
  return result;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void bytes_uint8_encode(uint8_t value, uint8_t *result) {
  result[0] = value;
}



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void bytes_uint16_encode(uint16_t value, uint8_t *result) {
  result[0] = value >>  8;
  result[1] = value;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void bytes_uint32_encode(uint32_t value, uint8_t *result) {
  result[0] = value >> 24;
  result[1] = value >> 16;
  result[2] = value >>  8;
  result[3] = value;
}


/* ------------------------------------------------------------------------
 * Strings are encoded as a uint8 len + len bytes of characters. Result must 
 * have enought space to accomodate this.
 * --------------------------------------------------------------------- */
void bytes_string_encode(const char* value, uint8_t *result) {
  uint8_t *ptr = result;
  uint8_t len = (uint8_t)strlen(value);

  bytes_uint8_encode(len, ptr);
  ptr++;

  for(int i=0;i<(int)len;i++) {
    ptr[i] = value[i];
  }

}


