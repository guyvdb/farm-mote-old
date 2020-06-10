// This file generates some on host tests

#include "framelst.h"
#include "framebuf.h"
#include "frame.h"

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>

/*
TEST_ASSERT_TRUE(condition)
TEST_ASSERT_FALSE(condition)  
TEST_ASSERT(condition)
TEST_ASSERT_UNLESS(condition)
TEST_FAIL()
TEST_FAIL_MESSAGE(message)

TEST_ASSERT_EQUAL_INT(expected, actual)
TEST_ASSERT_EQUAL_INT8(expected, actual)
TEST_ASSERT_EQUAL_INT16(expected, actual)
TEST_ASSERT_EQUAL_INT32(expected, actual)
TEST_ASSERT_EQUAL_INT64(expected, actual)

TEST_ASSERT_EQUAL_UINT(expected, actual)
TEST_ASSERT_EQUAL_UINT8(expected, actual)
TEST_ASSERT_EQUAL_UINT16(expected, actual)
TEST_ASSERT_EQUAL_UINT32(expected, actual)
TEST_ASSERT_EQUAL_UINT64(expected, actual)


TEST_ASSERT_EQUAL_HEX(expected, actual)
TEST_ASSERT_EQUAL_HEX8(expected, actual)
TEST_ASSERT_EQUAL_HEX16(expected, actual)
TEST_ASSERT_EQUAL_HEX32(expected, actual)
TEST_ASSERT_EQUAL_HEX64(expected, actual)


TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)
TEST_ASSERT_EQUAL_FLOAT(expected, actual)
TEST_ASSERT_EQUAL_DOUBLE(expected, actual)

TEST_ASSERT_EQUAL_STRING(expected, actual)
TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)
TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message)
TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expected, actual, len, message)

TEST_ASSERT_NULL(pointer)
TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)

*/

// Control Characters Are: 2, 3, 27


// Data without the need for escaping 

static uint8_t network_bytes[] =            {SFLAG,
                                             5,                                  // version
                                             0, 255,                             // id
                                             6,                                  // tcount
                                             0, 0, 0, 128,                       // timestamp
                                             0, 64,                              // refid
                                             32,                                 // cmd
                                             12,                                 // len
                                             4, 65, 66, 67, 68,                  // payload
                                             128, 255 , 35, 100, 50, 120, 128,   // payload
                                             EFLAG};

static uint8_t frame_bytes[] =              {
                                             5,                                  // version
                                             0, 255,                             // id
                                             6,                                  // tcount
                                             0, 0, 0, 128,                       // timestamp
                                             0, 64,                              // refid
                                             32,                                 // cmd
                                             11,                                 // len
                                             4, 65, 66, 67, 68,                  // payload
                                             128, 255 , 35, 100, 50, 120, 128    // payload
                                             };

static uint8_t version = 5;                 // {5}
static uint16_t id = 255;                   // {0, 255}
static uint8_t tcount = 6;                  // {6}
static int32_t timestamp = 128;             // {0, 0, 0, 128}
static uint16_t refid = 64;                 // {0, 64}
static uint8_t cmd = 32;                    // {32}
static uint8_t paylen = 12;                 // {11}
static uint8_t payload[] = {4,65,66,67,68, 0, 0, 0, 0, 0, 0, 0}; // uint8(4)ABCD 000000


// place some args into the payload
static char    *arg1 = "ABCD";                                               //5 bytes
static uint8_t  arg2 = 128;                                                  //1 byte 
static uint16_t arg3 = (255 << 8) + 35;                                      //2 bytes
static uint32_t arg4 = (100 << 24) + (50 << 16) + (120 << 8) + 128;          //4 bytes
                                                                             //12 bytes total 



//                         SFLAG   EFLAG    ESCAPE
// Control Characters Are:   2,      3,       27

static uint8_t network_bytes_esc[] = {2,                         //SFLAG
                                      27, 2,                     //version
                                      10, 27, 3,                 // id
                                      27, 27,                    // tcount    
                                      10, 27, 2, 11, 27, 27,     // timestamp
                                      0, 27, 27,                 // refid
                                      27, 3,                     // cmd
                                      11,                        // len 
                                      27, 3, 65, 66, 67,         // payload
                                      0, 0, 0, 0, 0, 0, 0,       // payload
                                      3                          //EFLAG
                                     };


static uint8_t frame_bytes_esc[] = {
};


static uint8_t version_esc = SFLAG;                                                  // {5}
static uint16_t id_esc = (10 << 8) + EFLAG ;                                         // {0, 255}
static uint8_t tcount_esc = ESCAPE;                                                  // {6}
static int32_t timestamp_esc = (10 << 24) + (SFLAG << 16) + (11 << 8) + ESCAPE;      // {0, 0, 0, 128}
static uint16_t refid_esc = (EFLAG << 8) + ESCAPE;                                   // {0, 64}
static uint8_t cmd_esc = EFLAG;                                                      // {32}
static uint8_t paylen_esc = 11;                                                      // {11}
static uint8_t payload_esc[] = {3,65,66,67, 0, 0, 0, 0, 0, 0, 0};                    // uint8(3)ABCD


// place some args into the payload
static char    *arg1_esc = "ABC";                                                //4 bytes
static uint8_t  arg2_esc = ESCAPE;                                               //1 byte 
static uint16_t arg3_esc = (EFLAG << 8) + SFLAG;                                 //2 bytes
static uint32_t arg4_esc = (EFLAG << 24) + (128 << 16) + (SFLAG << 8) + ESCAPE;  //4 bytes
                                                                                 //11 bytes total 
 
// |--------------|---------------|------------|---------------|-------------------|---------------|------------|-----------|--------------------|--------------|
// | SFLAG 1 byte | version 1byte | id 2 bytes | tcount 1 byte | timestamp 4 bytes | refid 2 bytes | cmd 1 byte | len 1byte | payload <varbytes> | EFLAG 1 byte |
// |--------------|---------------|------------|---------------|-------------------|---------------|------------|-----------|--------------------|--------------|
//


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void setUp (void) {
  framebuf_reset();
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void tearDown (void) {
  
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static frame_t *make_frame(void) {
  frame_t * frame = frame_create(cmd, refid, timestamp, paylen);
  frame->version = version;
  frame->id = id;
  frame->tcount = tcount;
  for(int i=0;i<sizeof(payload);i++) {
    frame->payload[i] = payload[i];
  }  
  return frame;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static frame_t *make_frame_esc(void) {
  frame_t *frame = frame_create(cmd_esc, refid_esc, timestamp_esc, paylen_esc);
  frame->version = version_esc;
  frame->id = id_esc;
  frame->tcount = tcount_esc;
  for(int i=0;i<=sizeof(payload_esc);i++) {
    frame->payload[i] = payload_esc[i];
  }  
  return frame;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void print_bytes(const char *msg, uint8_t *data, size_t len) {
  printf("%s [", msg);

  for(int i=0;i<len;i++) {
    if(i+1 == len) {
      printf("%d", data[i]);
    } else {
      printf("%d ", data[i]);
    }
  }

  printf("]\n");

}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_frame_create(void) {
  frame_t *frame = make_frame();
  TEST_ASSERT_NOT_NULL(frame);
  TEST_ASSERT_EQUAL_UINT8(cmd, frame->cmd); 
  TEST_ASSERT_EQUAL_UINT8(refid, frame->refid);
  TEST_ASSERT_EQUAL_INT32(timestamp, frame->timestamp);
  TEST_ASSERT_EQUAL_UINT8(paylen, frame->len);
  frame_free(frame);
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_frame_args(void) {
  uint8_t a1;
  uint16_t a2;
  uint32_t a3;
  char *a4;

  frame_t * frame = make_frame();
  TEST_ASSERT_NOT_NULL(frame);

  // this overwrites the args in the payload
  frame_args_begin(frame);
  TEST_ASSERT(frame_args_put_uint8(frame,123));
  TEST_ASSERT(frame_args_put_uint16(frame, 4567));
  TEST_ASSERT(frame_args_put_uint32(frame, 891011));
  TEST_ASSERT(frame_args_put_string(frame,"ABCD"));
  frame_args_end(frame);
  
  frame_args_begin(frame);
  TEST_ASSERT(frame_args_get_uint8(frame, &a1));
  TEST_ASSERT(frame_args_get_uint16(frame, &a2));
  TEST_ASSERT(frame_args_get_uint32(frame, &a3));
  //TEST_ASSERT(frame_args_get_string(frame, &arg4));
  frame_args_end(frame);
  
  TEST_ASSERT_EQUAL_UINT8(123, a1);
  TEST_ASSERT_EQUAL_UINT16(4567, a2);
  TEST_ASSERT_EQUAL_UINT32(891011, a3);
  //TEST_ASSERT_EQUAL_STRING("ABC", arg4);

  //free(arg4);
  frame_free(frame);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_frame_encode(void) {
  uint8_t buf[256];
  int len;
  int encoded;
  
  frame_t *frame = make_frame();
  TEST_ASSERT_NOT_NULL(frame);
  len = frame_encoded_len(frame);
  encoded = frame_encode(frame, buf, sizeof(buf));
  TEST_ASSERT_EQUAL_INT(encoded,len);
  frame_free(frame); 
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_frame_decode(void) {
  size_t len = sizeof(network_bytes);

  // write the frame to the framebuf
  framebuf_write(network_bytes, len);
 
  frame_t *frame = framebuf_deframe();
  TEST_ASSERT_NOT_NULL(frame);
  

  TEST_ASSERT_EQUAL_UINT8(cmd, frame->cmd);
  TEST_ASSERT_EQUAL_UINT8(refid,frame->refid); 
  TEST_ASSERT_EQUAL_INT32(timestamp,frame->timestamp); 
  TEST_ASSERT_EQUAL_UINT8(paylen,frame->len);
  
  frame_free(frame);

}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_frame_decode_escaped(void) {
  uint8_t buf[256];
  int len = sizeof(network_bytes_esc);

  framebuf_write(network_bytes_esc, len);

  frame_t *frame = framebuf_deframe();
  TEST_ASSERT_NOT_NULL(frame);

  TEST_ASSERT_EQUAL_UINT8(cmd_esc, frame->cmd);
  TEST_ASSERT_EQUAL_UINT8(refid_esc,frame->refid); 
  TEST_ASSERT_EQUAL_INT32(timestamp_esc,frame->timestamp); 
  TEST_ASSERT_EQUAL_UINT8(paylen_esc,frame->len);

  frame_free(frame);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_frame_encode_decode(void) {
  frame_t *f1, *f2;
  int encoded;
  uint8_t buf[256];
  
  
  f1 = make_frame();
  TEST_ASSERT_NOT_NULL(f1);


  encoded = frame_encode(f1, buf, sizeof(buf));
  framebuf_write(buf, encoded);

  f2= framebuf_deframe();
  TEST_ASSERT_NOT_NULL(f2);


// |--------------|---------------|------------|---------------|-------------------|---------------|------------|-----------|--------------------|--------------|
// | SFLAG 1 byte | version 1byte | id 2 bytes | tcount 1 byte | timestamp 4 bytes | refid 2 bytes | cmd 1 byte | len 1byte | payload <varbytes> | EFLAG 1 byte |
// |--------------|---------------|------------|---------------|-------------------|---------------|------------|-----------|--------------------|--------------|

  TEST_ASSERT_EQUAL_UINT8(f1->version, f2->version);
  TEST_ASSERT_EQUAL_UINT16(f1->id, f2->id);
  TEST_ASSERT_EQUAL_UINT8(f1->tcount, f2->tcount);
  TEST_ASSERT_EQUAL_INT32(f1->timestamp, f2->timestamp);
  TEST_ASSERT_EQUAL_UINT16(f1->refid, f2->refid);
  TEST_ASSERT_EQUAL_UINT8(f1->cmd, f2->cmd);
  TEST_ASSERT_EQUAL_UINT8(f1->len, f2->len);

  for(int i=0; i< (int)f1->len; i++) {
    TEST_ASSERT_EQUAL_UINT8(f1->payload[i], f2->payload[i]);
  }
  

  frame_free(f1);
  frame_free(f2);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_frame_to_string(void) {
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_framelst_add() {
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void test_framelst_remove() {
}



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void main(char **argv, int argc) {
    UNITY_BEGIN();
    RUN_TEST(test_frame_create);
    RUN_TEST(test_frame_args);
    RUN_TEST(test_frame_encode);
    RUN_TEST(test_frame_decode);
    RUN_TEST(test_frame_decode_escaped);
    RUN_TEST(test_frame_encode_decode);
    UNITY_END();
}

