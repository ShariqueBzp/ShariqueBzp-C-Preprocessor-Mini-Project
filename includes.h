#ifndef INCLUDES_H
#define INCLUDES_H

#include "common.h"

void add_include_dir(const char *dir);
void reset_include_dirs(void);
int find_include(const char *name, int quoted, char *out, int out_size);
int already_included(const char *path);
void mark_included(const char *path);
void reset_included(void);
void set_pending_include_dirs(const char *dirs[], int n);
void apply_pending_include_dirs(void);

#endif
