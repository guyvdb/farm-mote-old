#include "framelst.h"



static framelst_t framelist = {.head = 0x0};


// Walk the framelist and find the last frame
// return - frame_t point on success or 0x0 on fail
static frame_t *last() {
  frame_t *ptr;
  
  // if the list is empty return null 
  if(framelist.head == 0x0) {
    return 0x0;
  }

  ptr = framelist.head;

  while(ptr->next != 0x0) {
    ptr = ptr->next;
  }

  return ptr;
}


// Add a frame to the list
// return - 1 on success 0 on fail
int framelst_add(frame_t *frame) {
  
  // if the framelist is empty add it to the head 
  if(framelist.head == 0x0) {
    framelist.head = frame;
    return 1;
  }

  frame_t *ptr = last();
  if (ptr != 0x0) {
    ptr->next = frame;
    frame->prev = ptr;
    return 1;
  } else {
    return 0;
  }
}

// Remove a frame from the list 
int framelst_remove(frame_t *frame) {
  
  // is the frame at the head of the list 
  if(framelist.head == frame) {
    // is there more that one frame in the list?
    if(frame->next != 0x0) {
      frame->next->prev = 0x0;
      framelist.head = frame->next;
      frame->next = 0x0;
      frame->prev = 0x0;
      return 1;
    } else {
      framelist.head = 0x0;
      frame->next = 0x0;
      frame->prev = 0x0;
      return 1;
    }
  }

  // is the frame at the tail of the list 
  if(frame->next == 0x0) {
    frame->prev->next = 0x0;
    frame->next = 0x0;
    frame->prev = 0x;
    return 1;
  }

  // is the frame somewhere in the middle of the list
  if(frame->next != 0x0 && frame->prev != 0x0) {
    frame_t *prev = frame->prev;
    frame_t *next = frame->next;

    prev->next = next;
    next->prev = prev;

    frame->next = 0x0;
    frame->prev = 0x0;
    return 1;
  }

  // we could not find the frame
  return 0;
}

// Find a frame by id 
frame_t *framelst_find_by_id(uint8_t id) {
}

// Iterate over the frames by transmission time
void framelst_iterate_pastdue(int32_t olderthat, framelst_iteratorfn fn) {
}

// Find the first frame with a given command
frame_t *framelst_find_first_by_cmd(uint8_t cmd) {
}

// find the next frame with a given command
frame_t *framelst_find_next_by_cmd(frame_t *prev) {
}

