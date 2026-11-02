#include <stdio.h>
#include "all_points.h"

/* Object-like macros to test Point 3 */
#define PI 3.14159
#define GREETING "SecureEntry"

/* Function-like macro to test Point 4 — nested expansion through SQUARE */
#define AREA(r) (PI * SQUARE(r))

/* A macro used in a conditional test */
#define FEATURE_ON 1

int main(void)
{
    /* Point 1: this comment must disappear */
    int n = MAX;             /* MAX comes from all_points.h */
    float a = AREA(5);       /* nested: AREA -> PI * SQUARE */
    int s = SQUARE(7);       /* simple function-like */
    char *msg = GREETING;    /* string macro */

    // Another line comment to remove
    printf("%s n=%d a=%f s=%d\n", msg, n, a, s);

    /* Point 5: conditional compilation */
#ifdef FEATURE_ON
    printf("feature on\n");
#endif

#ifdef FEATURE_OFF
    printf("feature off - must NOT appear\n");
#endif

#ifndef FEATURE_OFF
    printf("feature off is undefined - should appear\n");
#endif

    return 0;
}
