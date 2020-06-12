#pragma once

//TODO the type cmdfunc should be changed to console_cmdfn
//TODO the type cmd should be changed to console_cmd
//TODO the type cmd_p should be dropped


typedef void (cmdfunc)(char *argv[], int argc);


struct cmd {
  char *name;
  void *next;
  void *prev;
  cmdfunc *func;
};
typedef struct cmd cmd_t;
typedef cmd_t* cmd_p;



cmd_p add_console_cmd(char *name, cmdfunc *func);
cmd_p find_console_cmd(char *name);


