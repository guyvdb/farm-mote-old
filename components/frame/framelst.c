#include "framelst.h"

#include <stdio.h>

static framelst_t framelist = {.head = 0x0};


// Walk the framelist and find the last frame
// return - frame_t point on success or 0x0 on fail
static frame_t *last() {
  frame_t *ptr;
  
  // if the list is empty return null 
  if(framelist.head == 0x0) {
    //    printf("last() head is 0x0\n");
    return 0x0;
  }

  // if the list only has one element
  if(framelist.head->next == 0x0) {
    //  printf("last() list only has one element");
    return framelist.head;
  }

  // list has more than one element 
  ptr = framelist.head;  
  while(ptr->next != 0x0) {
    ptr = ptr->next;
  }

  return ptr;
}

void framelst_reset(void) {
  framelist.head = 0x0;
}


// Add a frame to the list
// return - 1 on success 0 on fail
int framelst_add(frame_t *frame) {
  
  // if the framelist is empty add it to the head 
  if(framelist.head == 0x0) {
    framelist.head = frame;
    //printf("frame set on head\n");
    return 1;
  }

  frame_t *ptr = last();
  if (ptr != 0x0) {
    ptr->next = frame;
    frame->prev = ptr;
    //printf("frame set on last\n");
    return 1;
  } else {
    //printf("frame not set\n");
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
    frame->prev = 0x0;
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
  frame_t *ptr = framelist.head;


  // printf("find_by_id(%d)\n", id);
  while(ptr != 0x0) {
    if(ptr->id == id) {
      //  printf("0x%X matches id %d\n", id);
      return ptr;
    }
    ptr = ptr->next;
  }
  // printf("find_by_id - not found\n");
  return 0x0;  
}

// Iterate over the frames by transmission time
void framelst_iterate_pastdue(int32_t olderthat, framelst_iteratorfn fn) {
}

// Find the first frame with a given command
frame_t *framelst_find_first_by_cmd(uint8_t cmd) {
  frame_t *ptr = framelist.head;

  while(ptr != 0x0) {
    if(ptr->cmd == cmd) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return 0x0;
}

// find the next frame with a given command
frame_t *framelst_find_next_by_cmd(frame_t *prev) {
  uint8_t cmd = prev->cmd;
  frame_t *ptr = prev->next;
  while(ptr != 0x0) {
    if(ptr->cmd == cmd) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return 0x0;
}

void framelst_print(void) {
  frame_t *ptr = framelist.head;
  int pos = 0;
  printf("--- Frame List ---\n");

  while(ptr != 0x0) {
    printf("%d) 0x%X id=%d\n", pos, ptr, ptr->id);
    ptr = ptr->next;
  }
  
  printf("--- End List ---\n");
}
