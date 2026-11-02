#include <stdio.h>

// This is a line comment — should disappear
int a = 10;   // trailing comment

/* This is a
   multi-line block comment
   — should disappear too */

int b = 20;

int main(void)
{
    // Comment inside function
    printf("%d %d\n", a, b);  /* another comment */
    return 0;
}
