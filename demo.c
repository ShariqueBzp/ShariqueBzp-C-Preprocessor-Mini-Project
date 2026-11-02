#include "demo.h"

#define PI 3.14159
#define AREA(r) (PI * SQUARE(r))

int main(void)
{
    int n = MAX;
    // area of circle
    float a = AREA(5);
    //printf function
    printf("n=%d area=%f\n", n, a);
    return 0;
}
