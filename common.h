#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 128
#define MAX_PARAM 64
#define MAX_PARAMS 16
#define MAX_PATH 512
#define MAX_INCLUDE_DIRS 32
#define MAX_INCLUDED 512

void die(const char *msg);
void *xmalloc(int size);
char *xstrdup(const char *s);

#endif
