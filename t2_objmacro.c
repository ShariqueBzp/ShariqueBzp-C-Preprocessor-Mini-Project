#include <stdio.h>

#define MAX 100
#define NAME "SecureEntry"
#define FLAG 1

int main(void)
{
    int n = MAX;
    int m = MAX + MAX;
    char *s = NAME;

    if (FLAG) {
        printf("%s: n=%d m=%d\n", s, n, m);
    }

    return 0;
}
