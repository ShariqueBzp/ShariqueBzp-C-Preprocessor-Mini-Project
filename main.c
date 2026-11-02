#include "preprocessor.h"
#include "includes.h"
#include "common.h"

static void usage(const char *prog)
{
    printf("Usage: %s <input.c> [-I<dir>]... [-o output.i] [-v]\n",
           prog);
}

int main(int argc, char **argv)
{
    const char *input = NULL;
    const char *output = NULL;
    int verbose = 0;

    const char *indirs[MAX_INCLUDE_DIRS];
    int n_indirs = 0;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-I", 2) == 0) {
            const char *d = argv[i] + 2;

            if (n_indirs < MAX_INCLUDE_DIRS)
                indirs[n_indirs++] = (*d != '\0') ? d : ".";

            continue;
        }

        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output = argv[++i];
            continue;
        }

        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
            continue;
        }

        if (argv[i][0] == '-') {
            usage(argv[0]);
            return 1;
        }

        input = argv[i];
    }

    if (input == NULL) {
        usage(argv[0]);
        return 1;
    }

    set_pending_include_dirs(indirs, n_indirs);

    return run_preprocessor(input, output, verbose);
}
