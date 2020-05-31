#pragma once

// Which interface can respond to this type of command 
//enum { NOINTERFACE = 0x1, SOCKETINTERFACE = 0x2, CONSOLEINTERFACE = 0x4 };



//typedef int (*printfunc)(const char *format, ...);
typedef void (cmdfunc)(char *argv[], int argc);
//typedef int cmd_interface;

struct cmd {
  char *name;
  void *next;
  void *prev;
  cmdfunc *func;
};
typedef struct cmd cmd_t;
typedef cmd_t* cmd_p;







void initialize_console_commands(void);
void finalize_console_commands(void);

cmd_p add_console_cmd(char *name, cmdfunc *func);
cmd_p find_console_cmd(char *name);
//cmd_p find_cmd_for_interface(char *name, cmd_interface interface);
//printfunc find_cmd_print_func(cmd_interface interface);
