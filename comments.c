#include "comments.h"
#include "common.h"

char *strip_comments(const char *src)
{
    int n = (int)strlen(src);
    char *out = (char *)xmalloc(n + 1);

    int i = 0;
    int j = 0;
    int in_string = 0;
    int in_char = 0;

    while (i < n) {
        char c = src[i];

        if (in_string) {
            out[j++] = c;

            if (c == '\\' && i + 1 < n) {
                out[j++] = src[i + 1];
                i += 2;
                continue;
            }

            if (c == '"')
                in_string = 0;

            i++;
            continue;
        }

        if (in_char) {
            out[j++] = c;

            if (c == '\\' && i + 1 < n) {
                out[j++] = src[i + 1];
                i += 2;
                continue;
            }

            if (c == '\'')
                in_char = 0;

            i++;
            continue;
        }

        if (c == '"') {
            in_string = 1;
            out[j++] = c;
            i++;
            continue;
        }

        if (c == '\'') {
            in_char = 1;
            out[j++] = c;
            i++;
            continue;
        }

        if (c == '/' && i + 1 < n && src[i + 1] == '/') {
            while (i < n && src[i] != '\n')
                i++;
            continue;
        }

        if (c == '/' && i + 1 < n && src[i + 1] == '*') {
            i += 2;

            while (i + 1 < n &&
                   !(src[i] == '*' && src[i + 1] == '/')) {
                if (src[i] == '\n')
                    out[j++] = '\n';
                i++;
            }

            if (i + 1 < n)
                i += 2;

            continue;
        }

        out[j++] = c;
        i++;
    }

    out[j] = '\0';
    return out;
}
