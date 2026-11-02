#include "preprocessor.h"
#include "utils.h"
#include "macro.h"
#include "comments.h"
#include "includes.h"
#include "expand.h"

#include <string.h>

typedef struct {
    char *data;
    int len;
    int cap;
} Buffer;

static void buf_init(Buffer *b)
{
    b->cap = 4096;
    b->len = 0;
    b->data = (char *)xmalloc(b->cap);
    b->data[0] = '\0';
}

static void buf_ensure(Buffer *b, int need)
{
    while (b->len + need + 1 >= b->cap) {
        b->cap *= 2;
        b->data = (char *)realloc(b->data, b->cap);
        if (b->data == NULL)
            die("out of memory");
    }
}

static void buf_add(Buffer *b, const char *s)
{
    int n = (int)strlen(s);
    buf_ensure(b, n);
    for (int i = 0; i < n; i++)
        b->data[b->len++] = s[i];
    b->data[b->len] = '\0';
}

static void buf_add_char(Buffer *b, char c)
{
    buf_ensure(b, 1);
    b->data[b->len++] = c;
    b->data[b->len] = '\0';
}

static void buf_free(Buffer *b)
{
    free(b->data);
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

/* Emit a GCC-style line marker:  # <line> "<file>" [flags] */
static void emit_line_marker(Buffer *b, int line, const char *file, const char *flags)
{
    char marker[1024];

    if (flags != NULL && *flags != '\0')
        snprintf(marker, sizeof(marker), "# %d \"%s\" %s\n", line, file, flags);
    else
        snprintf(marker, sizeof(marker), "# %d \"%s\"\n", line, file);

    buf_add(b, marker);
}

char *process_file(const char *path);

/* =============================================================
 * Guards against runaway includes
 * ============================================================= */
#define MAX_INCLUDE_DEPTH 30
static int g_include_depth = 0;

#define MAX_FILES_TOTAL 500
static int g_files_processed = 0;

static void do_include(const char *arg_text, Buffer *out,
                       const char *parent_file, int parent_line)
{
    while (*arg_text == ' ' || *arg_text == '\t')
        arg_text++;

    int quoted = 0;
    char end_char = 0;

    if (*arg_text == '"') {
        quoted = 1;
        end_char = '"';
        arg_text++;
    } else if (*arg_text == '<') {
        quoted = 0;
        end_char = '>';
        arg_text++;
    }

    char header[MAX_PATH];
    int hk = 0;

    while (*arg_text != '\0' &&
           *arg_text != end_char &&
           hk < MAX_PATH - 1) {
        header[hk++] = *arg_text++;
    }

    header[hk] = '\0';

    char resolved[MAX_PATH];

    if (!find_include(header, quoted, resolved, MAX_PATH)) {
        fprintf(stderr, "TRACE: cannot find '%s'\n", header);

        buf_add(out, "#include ");
        buf_add_char(out, quoted ? '"' : '<');
        buf_add(out, header);
        buf_add_char(out, quoted ? '"' : '>');
        buf_add(out, "\n");
        return;
    }

    if (g_include_depth >= MAX_INCLUDE_DEPTH) {
        fprintf(stderr, "TRACE: depth > %d at '%s'\n",
                MAX_INCLUDE_DEPTH, header);

        buf_add(out, "#include ");
        buf_add_char(out, quoted ? '"' : '<');
        buf_add(out, header);
        buf_add_char(out, quoted ? '"' : '>');
        buf_add(out, "\n");
        return;
    }

    g_include_depth++;

    emit_line_marker(out, 1, resolved, "1");

    fprintf(stderr, "TRACE: include '%s' -> '%s' depth=%d\n",
            header, resolved, g_include_depth);

    char *sub = process_file(resolved);
    buf_add(out, sub);
    buf_add(out, "\n");
    free(sub);

    emit_line_marker(out, parent_line + 1, parent_file, "2");

    g_include_depth--;
}

char *process_file(const char *path)
{
    if (g_files_processed >= MAX_FILES_TOTAL) {
        fprintf(stderr, "TRACE: file count limit at '%s'\n", path);
        return xstrdup("");
    }
    g_files_processed++;

    if (already_included(path)) {
        return xstrdup("");
    }

    mark_included(path);

    fprintf(stderr, "TRACE: process_file('%s') #%d\n", path, g_files_processed);

    fprintf(stderr, "TRACE: %s -> about to read_file\n", path);
    char *raw = read_file(path);
    fprintf(stderr, "TRACE: %s -> read_file done (%d bytes)\n",
            path, raw ? (int)strlen(raw) : -1);

    if (raw == NULL) {
        fprintf(stderr, "TRACE: cannot open '%s'\n", path);
        return xstrdup("");
    }

    fprintf(stderr, "TRACE: %s -> about to strip_comments\n", path);
    char *clean = strip_comments(raw);
    fprintf(stderr, "TRACE: %s -> strip_comments done (%d bytes)\n",
            path, (int)strlen(clean));
    free(raw);

    Buffer out;
    buf_init(&out);

    int i = 0;
    int n = (int)strlen(clean);

    int active = 1;
    int condition_stack[64];
    int condition_top = 0;
    int line_number = 1;

    fprintf(stderr, "TRACE: %s -> entering line loop (%d total chars)\n",
            path, n);

    while (i < n) {
        int start = i;
        int this_line = line_number;

        if (this_line % 50 == 0) {
            fprintf(stderr, "TRACE: %s -> line %d\n", path, this_line);
        }

        while (i < n && clean[i] != '\n')
            i++;

        int end = i;

        char line[MAX_LINE];
        int len = end - start;

        if (len >= MAX_LINE)
            len = MAX_LINE - 1;

        for (int t = 0; t < len; t++)
            line[t] = clean[start + t];

        line[len] = '\0';

        if (i < n && clean[i] == '\n') {
            i++;
            line_number++;
        }

        int p = 0;

        while (line[p] == ' ' || line[p] == '\t')
            p++;

        if (line[p] == '#') {
            p++;

            while (line[p] == ' ' || line[p] == '\t')
                p++;

            if (strncmp(line + p, "ifndef", 6) == 0) {
                char name[MAX_NAME];
                sscanf(line + p + 6, "%127s", name);

                if (condition_top < 64)
                    condition_stack[condition_top++] = active;

                active = active && (macro_find(name) == NULL);
            }
            else if (strncmp(line + p, "ifdef", 5) == 0) {
                char name[MAX_NAME];
                sscanf(line + p + 5, "%127s", name);

                if (condition_top < 64)
                    condition_stack[condition_top++] = active;

                active = active && (macro_find(name) != NULL);
            }
            else if (strncmp(line + p, "endif", 5) == 0) {
                if (condition_top > 0)
                    active = condition_stack[--condition_top];
            }
            else if (!active) {
                /* Skip inactive */
            }
            else if (strncmp(line + p, "include", 7) == 0) {
                do_include(line + p + 7, &out, path, this_line);
            }
            else if (strncmp(line + p, "define", 6) == 0) {
                fprintf(stderr, "TRACE: %s -> parse_define line %d: %.50s\n",
                        path, this_line, line);
                parse_define(line + p + 6);
            }
            else if (strncmp(line + p, "undef", 5) == 0) {
                parse_undef(line + p + 5);
            }
            else {
                buf_add(&out, line);
                buf_add(&out, "\n");
            }
        }
        else if (active) {
            if (this_line % 50 == 0) {
                fprintf(stderr, "TRACE: %s -> expand_line line %d: %.30s\n",
                        path, this_line, line);
            }
            char *expanded = expand_line(line);
            buf_add(&out, expanded);
            buf_add(&out, "\n");
            free(expanded);
        }
    }

    free(clean);

    char *result = out.data;
    out.data = NULL;
    buf_free(&out);

    fprintf(stderr, "TRACE: %s -> returning %d bytes\n",
            path, result ? (int)strlen(result) : -1);

    return result;
}

int run_preprocessor(const char *input_file,
                     const char *output_file,
                     int verbose)
{
    fprintf(stderr, "TRACE: run_preprocessor entered\n");

    macro_list_clear();
    reset_include_dirs();
    reset_included();

    g_include_depth = 0;
    g_files_processed = 0;

    add_include_dir("sysheaders");
    add_include_dir("/usr/include");
    add_include_dir("/usr/include/x86_64-linux-gnu");
    add_include_dir("/usr/lib/gcc/x86_64-linux-gnu/13/include");
    add_include_dir(".");
    apply_pending_include_dirs();

    fprintf(stderr, "TRACE: include dirs set up\n");

    {
        const char *last_slash = strrchr(input_file, '/');
        if (last_slash != NULL) {
            char input_dir[MAX_PATH];
            size_t dir_len = (size_t)(last_slash - input_file);
            if (dir_len > 0 && dir_len < MAX_PATH) {
                memcpy(input_dir, input_file, dir_len);
                input_dir[dir_len] = '\0';
                add_include_dir(input_dir);
            }
        }
    }

    char outpath[MAX_PATH];
    if (output_file != NULL) {
        strncpy(outpath, output_file, MAX_PATH - 1);
        outpath[MAX_PATH - 1] = '\0';
    } else {
        make_output_name(input_file, outpath, MAX_PATH);
    }

    fprintf(stderr, "TRACE: outpath='%s'\n", outpath);

    Buffer init_markers;
    buf_init(&init_markers);

    emit_line_marker(&init_markers, 0, input_file, NULL);
    emit_line_marker(&init_markers, 0, "<built-in>", NULL);
    emit_line_marker(&init_markers, 0, "<command-line>", NULL);
    emit_line_marker(&init_markers, 1, input_file, "1");

    fprintf(stderr, "TRACE: preamble %d bytes, calling process_file('%s')\n",
            init_markers.len, input_file);

    char *result = process_file(input_file);

    fprintf(stderr, "TRACE: process_file returned %d bytes\n",
            result ? (int)strlen(result) : -1);

    Buffer final;
    buf_init(&final);
    buf_add(&final, init_markers.data);
    buf_add(&final, result);

    buf_free(&init_markers);
    free(result);

    char *final_data = final.data;
    final.data = NULL;
    buf_free(&final);

    fprintf(stderr, "TRACE: writing %d bytes to '%s'\n",
            final_data ? (int)strlen(final_data) : -1, outpath);

    write_file(outpath, final_data);
    free(final_data);

    fprintf(stderr, "TRACE: DONE\n");

    if (verbose)
        printf("[my preprocessor] Wrote extended source to: %s\n", outpath);

    return 0;
}
