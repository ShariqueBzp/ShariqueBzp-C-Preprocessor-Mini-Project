#ifndef MACRO_H
#define MACRO_H

#include "common.h"

typedef struct Macro {
    char name[MAX_NAME];
    int is_function;
    char params[MAX_PARAMS][MAX_PARAM];
    int nparams;
    char body[MAX_LINE];
    struct Macro *next;
} Macro;

void macro_list_clear(void);
Macro *macro_find(const char *name);
void macro_add(const char *name, int is_function,
               char params[][MAX_PARAM], int nparams,
               const char *body);
void macro_remove(const char *name);

#endif
