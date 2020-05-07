#pragma once




// A command is something that can be executed.


// The function signiture of all commands 
typedef int (*command_func_t)(int argc, char **argv);


// The definition of a command 
typedef struct {
  const char *command; 
  command_func_t func;
} command_t;

