#ifndef ALL_POINTS_H
#define ALL_POINTS_H

/* These macros must be consumed (not emitted) and available for expansion */
#define MAX 100
#define SQUARE(x) ((x) * (x))

/* This is the visible body of the header — must be pasted into the output */
int helper_from_header(void);

#endif
