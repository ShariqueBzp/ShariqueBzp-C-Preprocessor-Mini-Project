#ifndef UTILS_H
#define UTILS_H

#include "common.h"

int is_ident_start(char c);
int is_ident_char(char c);
void rstrip(char *s);
char *skip_spaces(char *s);
char *read_file(const char *path);
void write_file(const char *path, const char *text);
void make_output_name(const char *in, char *out, int out_size);

#endif
