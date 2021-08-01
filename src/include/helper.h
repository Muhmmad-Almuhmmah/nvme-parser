#ifndef _HELPER__H_
#define _HELPER__H_

#include <stdbool.h>

bool __file_exists(char *filename);
void __show_usage(void);
char *__find_block(void);
char *__exec_sh(char *cmd);

#endif
