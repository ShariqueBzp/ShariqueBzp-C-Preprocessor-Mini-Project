#include "expand.h"
#include "macro.h"
#include "utils.h"

static void expand_object_into(const char *line, char *out, int out_size)
{
    int i = 0;
    int j = 0;
    int in_string = 0;
    int in_char = 0;

    while (line[i] != '\0') {
        char c = line[i];

        if (in_string) {
            if (j < out_size - 1) out[j++] = c;
            if (c == '\\' && line[i + 1] != '\0') {
                if (j < out_size - 1) out[j++] = line[i + 1];
                i += 2;
                continue;
            }
            if (c == '"') in_string = 0;
            i++;
            continue;
        }

        if (in_char) {
            if (j < out_size - 1) out[j++] = c;
            if (c == '\\' && line[i + 1] != '\0') {
                if (j < out_size - 1) out[j++] = line[i + 1];
                i += 2;
                continue;
            }
            if (c == '\'') in_char = 0;
            i++;
            continue;
        }

        if (c == '"') {
            in_string = 1;
            if (j < out_size - 1) out[j++] = c;
            i++;
            continue;
        }
        if (c == '\'') {
            in_char = 1;
            if (j < out_size - 1) out[j++] = c;
            i++;
            continue;
        }

        if (is_ident_start(c)) {
            char name[MAX_NAME];
            int k = 0;

            while (is_ident_char(line[i]) && k < MAX_NAME - 1)
                name[k++] = line[i++];

            name[k] = '\0';

            Macro *m = macro_find(name);

            if (m != NULL && m->is_function == 0) {
                int b = 0;
                while (m->body[b] != '\0') {
                    if (j >= out_size - 1) goto done;
                    out[j++] = m->body[b++];
                }
            } else {
                for (int t = 0; t < k; t++) {
                    if (j >= out_size - 1) goto done;
                    out[j++] = name[t];
                }
            }
            continue;
        }

        if (j >= out_size - 1) break;
        out[j++] = c;
        i++;
    }

done:
    out[j] = '\0';
}

static void substitute_param(const char *src, const char *name,
                             const char *value, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    int name_len = (int)strlen(name);

    while (src[i] != '\0' && j < dst_size - 1) {
        if (is_ident_start(src[i]) &&
            strncmp(src + i, name, name_len) == 0 &&
            !is_ident_char(src[i + name_len])) {

            int k = 0;

            while (value[k] != '\0' && j < dst_size - 1)
                dst[j++] = value[k++];

            i += name_len;
        } else {
            dst[j++] = src[i++];
        }
    }

    dst[j] = '\0';
}

static int read_args(const char *line, int *pos,
                     char args[][MAX_LINE], int max_args)
{
    int i = *pos;
    int count = 0;
    int arg_len = 0;
    int depth = 1;

    if (line[i] != '(')
        return -1;

    i++;

    while (line[i] != '\0' && depth > 0) {
        char c = line[i];

        if (c == '(') {
            depth++;
            if (count < max_args && arg_len < MAX_LINE - 1)
                args[count][arg_len++] = c;
        } else if (c == ')') {
            depth--;
            if (depth == 0) {
                if (count < max_args) {
                    args[count][arg_len] = '\0';
                    count++;
                }
                i++;
                break;
            }
            if (count < max_args && arg_len < MAX_LINE - 1)
                args[count][arg_len++] = c;
        } else if (c == ',' && depth == 1) {
            if (count < max_args) {
                args[count][arg_len] = '\0';
                count++;
            }
            arg_len = 0;
            if (count >= max_args)
                break;
        } else {
            if (count < max_args && arg_len < MAX_LINE - 1)
                args[count][arg_len++] = c;
        }

        i++;
    }

    *pos = i;
    return count;
}

static void expand_function_into(const char *line, char *out, int out_size)
{
    int i = 0;
    int j = 0;

    while (line[i] != '\0' && j < out_size - 1) {
        char c = line[i];

        if (is_ident_start(c)) {
            char name[MAX_NAME];
            int k = 0;

            while (is_ident_char(line[i]) && k < MAX_NAME - 1)
                name[k++] = line[i++];

            name[k] = '\0';

            Macro *m = macro_find(name);

            if (m != NULL && m->is_function == 1) {
                int save = i;

                while (line[i] == ' ' || line[i] == '\t')
                    i++;

                if (line[i] == '(') {
                    char args[MAX_PARAMS][MAX_LINE];
                    int n = read_args(line, &i, args, MAX_PARAMS);

                    if (n == m->nparams) {
                        char expanded[MAX_LINE];

                        strncpy(expanded, m->body, MAX_LINE - 1);
                        expanded[MAX_LINE - 1] = '\0';

                        for (int p = 0; p < m->nparams; p++) {
                            char temp[MAX_LINE];

                            substitute_param(expanded,
                                             m->params[p],
                                             args[p],
                                             temp,
                                             MAX_LINE);

                            strncpy(expanded, temp, MAX_LINE - 1);
                            expanded[MAX_LINE - 1] = '\0';
                        }

                        int e = 0;

                        while (expanded[e] != '\0' && j < out_size - 1)
                            out[j++] = expanded[e++];

                        continue;
                    }
                }

                i = save;
            }

            for (int t = 0; t < k && j < out_size - 1; t++)
                out[j++] = name[t];

            continue;
        }

        out[j++] = c;
        i++;
    }

    out[j] = '\0';
}

char *expand_line(const char *line)
{
    int size = (int)strlen(line) * 32 + 16384;

    char *current = (char *)xmalloc(size);
    strncpy(current, line, size - 1);
    current[size - 1] = '\0';

    for (int pass = 0; pass < 20; pass++) {
        char *stage1 = (char *)xmalloc(size);
        char *stage2 = (char *)xmalloc(size);

        memset(stage1, 0, size);
        memset(stage2, 0, size);

        expand_object_into(current, stage1, size);
        expand_function_into(stage1, stage2, size);

        free(stage1);

        if (strcmp(current, stage2) == 0) {
            free(current);
            return stage2;
        }

        free(current);
        current = stage2;
    }

    return current;
}

void parse_define(const char *rest)
{
    rest = skip_spaces((char *)rest);

    char name[MAX_NAME];
    int k = 0;

    while (is_ident_char(*rest) && k < MAX_NAME - 1)
        name[k++] = *rest++;

    name[k] = '\0';

    if (k == 0)
        return;

    if (*rest == '(') {
        rest++;

        char params[MAX_PARAMS][MAX_PARAM];
        int nparams = 0;

        while (*rest != '\0' && *rest != ')') {
            while (*rest == ' ' || *rest == '\t')
                rest++;

            if (*rest == ')')
                break;

            /* Variadic: skip the ellipsis ... to avoid infinite loop */
            if (*rest == '.') {
                if (rest[1] == '.' && rest[2] == '.') {
                    rest += 3;
                    continue;
                }
                rest++;
                continue;
            }

            char pname[MAX_PARAM];
            int pk = 0;

            while (is_ident_char(*rest) && pk < MAX_PARAM - 1)
                pname[pk++] = *rest++;

            pname[pk] = '\0';

            if (pk > 0 && nparams < MAX_PARAMS) {
                strcpy(params[nparams], pname);
                nparams++;
            }

            while (*rest == ' ' || *rest == '\t')
                rest++;

            if (*rest == ',')
                rest++;
        }

        if (*rest == ')')
            rest++;

        while (*rest == ' ' || *rest == '\t')
            rest++;

        char body[MAX_LINE];

        strncpy(body, rest, MAX_LINE - 1);
        body[MAX_LINE - 1] = '\0';

        rstrip(body);

        macro_add(name, 1, params, nparams, body);
    } else {
        while (*rest == ' ' || *rest == '\t')
            rest++;

        char body[MAX_LINE];

        strncpy(body, rest, MAX_LINE - 1);
        body[MAX_LINE - 1] = '\0';

        rstrip(body);

        macro_add(name, 0, NULL, 0, body);
    }
}

void parse_undef(const char *rest)
{
    rest = skip_spaces((char *)rest);

    char name[MAX_NAME];
    int k = 0;

    while (is_ident_char(*rest) && k < MAX_NAME - 1)
        name[k++] = *rest++;

    name[k] = '\0';

    if (k > 0)
        macro_remove(name);
}
