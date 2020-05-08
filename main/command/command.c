
#include "command.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


cmd_p root_command;
cmd_p last_command;



static void freecmd(cmd_p cmd) {
  free(cmd->name);
  free(cmd);
}


void initialize_commands(void) {
  root_command = (cmd_p)malloc(sizeof(cmd_t));
  root_command->name = (char*)malloc(5);
  strcpy(root_command->name, "ROOT");
  root_command->prev = 0x0;
  root_command->next = 0x0;
  root_command->func = 0x0;
  root_command->interface = NOINTERFACE;

  last_command = root_command;
}


void finalize_commands(void) {
  // delete all the commands 

  cmd_p prev;
  cmd_p current = last_command;


  while(current != 0x0) {
    if(current->prev != 0x0) {
      prev = (cmd_p)current->prev;
    } else {
      prev = 0x0;
    }

    freecmd(current);
    if (prev != 0x0) {
      current = prev;
    } else {
      current = 0x0;
    }
  }

}




cmd_p add_cmd(char *name, cmdfunc *func, cmd_interface interface) {
  // set up the new command
  cmd_p cmd = (cmd_p)malloc(sizeof(cmd_t));
  cmd->next = 0x0;
  cmd->prev = 0x0;
  cmd->name = (char *)malloc(strlen(name));
  strcpy(cmd->name, name);
  cmd->func = func;
  cmd->interface = interface;

  // the current last_command->next must now == new command
  last_command->next = (void*)cmd ;
  // the new command->prev must now == last command
  cmd->prev = (void*)last_command;    
  // last command must == new command 
  last_command = cmd;
  
  return cmd;
}



cmd_p find_cmd(char *name) {
  cmd_p ptr = root_command;

  while(1) {  
    if( strcmp(name, ptr->name) == 0) {
      return ptr;
    }

    if(ptr->next != 0x0) {
      ptr = (cmd_p) ptr->next;
    } else {
      return 0x0;
    } 
  }
}
