#ifndef EXPAND_H
#define EXPAND_H

char *expand_line(const char *line);
void parse_define(const char *rest);
void parse_undef(const char *rest);

#endif
