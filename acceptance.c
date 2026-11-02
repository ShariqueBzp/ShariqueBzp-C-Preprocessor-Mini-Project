#include <stdio.h>
#include <stdlib.h>
#include "toolkit.h"
#include "mathx.h"

#define BUFFER_SIZE     256
#define DEFAULT_VALUE   42
#define SEPARATOR       "\n---\n"

#define SQUARE(x)       ((x) * (x))
#define CUBE(x)         (SQUARE(x) * (x))
#define GREETING(name)  "Hello, " name "!"

#define FEATURE_LOGGING 1
#define FEATURE_DEBUG   1
#define FEATURE_EXPERIMENTAL 0

int main(void)
{
    // line comment
    /* block comment */
    int buffer[BUFFER_SIZE];
    int count = ARRAY_LEN(buffer);
    int initial = DEFAULT_VALUE;

    double area = CIRCLE_AREA(5.0);
    int scaled = SCALE(21);
    int sq = SQUARE(7);
    int cb = CUBE(3);
    int smaller = MIN(15, 25);
    int larger = MAX2(15, 25);

    int result = SQUARE(SQUARE(3));

    printf("count=%d initial=%d\n", count, initial);
    printf("area=%f scaled=%d sq=%d cb=%d\n", area, scaled, sq, cb);
    printf("min=%d max=%d result=%d\n", smaller, larger, result);
    printf("%s", SEPARATOR);
    printf(GREETING("Vector India"));
    printf("\n");

#ifdef FEATURE_LOGGING
    printf("[LOG] logging enabled\n");
#endif

#ifdef FEATURE_DEBUG
    printf("[DBG] debug enabled\n");
#endif

#ifdef FEATURE_EXPERIMENTAL
    printf("[EXP] experimental - should NOT appear\n");
#endif

#ifndef FEATURE_EXPERIMENTAL
    printf("[OK] experimental is disabled\n");
#endif

    printf("formatted: %d %d %d\n", sq, cb, result);

    return 0;
}
