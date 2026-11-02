#include <stdio.h>
#include "mypredef.h"

int main(void)
{
    printf("Tool: %s v%s\n", TOOL_NAME, TOOL_VERSION);

    int arr[10];
    int n = ARRAY_SIZE(arr);

    printf("array size = %d\n", n);

    return 0;
}
