#pragma once

// Which interface can respond to this type of command 
enum { HTTPCOMMAND = 01, CONSOLECOMMAND = 02 };

typedef void (*cmdfunc)(char **argv, int argc);

struct cmd  {
  char *name;
  void *next;
  void *prev;
  cmdfunc *func;
  int interfaces;
} cmd_t;


cmd_t *add_cmd(char *name, cmdfunc *func, int interfaces);
