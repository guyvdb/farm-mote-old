#pragma once

#include "frame.h"


struct framelst {
  frame_t *head;
};

typedef struct framelst framelst_t;

typedef void (framelst_iteratorfn)(frame_t *frame);

// Reset the framelst
void framelst_reset(void);

// Add a frame to the list 
int framelst_add(frame_t *frame);

// Remove a frame from the list 
int framelst_remove(frame_t *frame);

// Find a frame by id 
frame_t *framelst_find_by_id(uint8_t id);

// Iterate over the frames by transmission time
void framelst_iterate_pastdue(int32_t olderthat, framelst_iteratorfn fn);

// Find the first frame with a given command
frame_t *framelst_find_first_by_cmd(uint8_t cmd);

// find the next frame with a given command
frame_t *framelst_find_next_by_cmd(frame_t *prev);


void framelst_print(void);
