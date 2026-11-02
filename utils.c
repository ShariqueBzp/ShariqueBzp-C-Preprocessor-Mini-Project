#include "utils.h"

void die(const char *msg)
{
    fprintf(stderr, "mypreprocessor: %s\n", msg);
    exit(1);
}

void *xmalloc(int size)
{
    void *p = malloc(size);
    if (p == NULL)
        die("out of memory");
    return p;
}

char *xstrdup(const char *s)
{
    int n = (int)strlen(s) + 1;
    char *p = (char *)xmalloc(n);
    strcpy(p, s);
    return p;
}

int is_ident_start(char c)
{
    return isalpha((unsigned char)c) || c == '_';
}

int is_ident_char(char c)
{
    return isalnum((unsigned char)c) || c == '_';
}

void rstrip(char *s)
{
    int n = (int)strlen(s);
    while (n > 0 &&
           (s[n - 1] == '\n' || s[n - 1] == '\r' ||
            s[n - 1] == ' ' || s[n - 1] == '\t')) {
        s[--n] = '\0';
    }
}

char *skip_spaces(char *s)
{
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}

char *read_file(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < 0) {
        fclose(f);
        return NULL;
    }

    char *buf = (char *)xmalloc((int)size + 1);
    int got = (int)fread(buf, 1, (int)size, f);
    buf[got] = '\0';

    fclose(f);
    return buf;
}

void write_file(const char *path, const char *text)
{
    FILE *f = fopen(path, "w");
    if (f == NULL)
        die("cannot open output file");

    fputs(text, f);
    fclose(f);
}

void make_output_name(const char *in, char *out, int out_size)
{
    int i = 0;

    while (in[i] != '\0' && i < out_size - 3) {
        out[i] = in[i];
        i++;
    }

    out[i] = '\0';

    int dot = -1;
    int slash = -1;

    for (int k = 0; k < i; k++) {
        if (out[k] == '/')
            slash = k;
        if (out[k] == '.')
            dot = k;
    }

    if (dot > slash)
        out[dot] = '\0';

    strcat(out, ".i");
}
