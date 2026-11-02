#include "macro.h"

static Macro *head = NULL;

void macro_list_clear(void)
{
    Macro *p = head;

    while (p != NULL) {
        Macro *next = p->next;
        free(p);
        p = next;
    }

    head = NULL;
}

Macro *macro_find(const char *name)
{
    Macro *p = head;

    while (p != NULL) {
        if (strcmp(p->name, name) == 0)
            return p;
        p = p->next;
    }

    return NULL;
}

void macro_add(const char *name, int is_function,
               char params[][MAX_PARAM], int nparams,
               const char *body)
{
    macro_remove(name);

    Macro *m = (Macro *)xmalloc(sizeof(Macro));

    strncpy(m->name, name, MAX_NAME - 1);
    m->name[MAX_NAME - 1] = '\0';

    m->is_function = is_function;
    m->nparams = nparams;

    for (int i = 0; i < nparams; i++) {
        strncpy(m->params[i], params[i], MAX_PARAM - 1);
        m->params[i][MAX_PARAM - 1] = '\0';
    }

    strncpy(m->body, body, MAX_LINE - 1);
    m->body[MAX_LINE - 1] = '\0';

    m->next = head;
    head = m;
}

void macro_remove(const char *name)
{
    Macro *prev = NULL;
    Macro *cur = head;

    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) {
            if (prev == NULL)
                head = cur->next;
            else
                prev->next = cur->next;

            free(cur);
            return;
        }

        prev = cur;
        cur = cur->next;
    }
}
