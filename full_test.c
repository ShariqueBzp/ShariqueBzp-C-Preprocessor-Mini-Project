#include "helper.h"

#define PI 3.14159
#define AREA(r) (PI * SQUARE(r))

#define DEBUG 1

int main(void)
{
    int n = MAX;                    /* uses MAX from helper.h */
    float a = AREA(5);              /* uses AREA + SQUARE */
    int sq = SQUARE(7);

    printf("n=%d area=%f sq=%d\n", n, a, sq);

#ifdef DEBUG
    printf("debug on\n");
#endif

#ifdef PRODUCTION
    printf("production\n");
#endif

#undef MAX
#define MAX 200

    int big = MAX;

    printf("big=%d\n", big);

    return 0;
}
