/** @todo: Add Microsoft license information */

#include "getopt.h"
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

char* optarg = NULL;
int optind = 1;

int getopt(int argc, char *const argv[], const char *optstring)
{
    if ((optind >= argc) || (argv[optind][0] != '-') || (argv[optind][0] == 0))
    {
        return -1;
    }

    int opt = argv[optind][1];
    const char *p = strchr(optstring, opt);

    if (p == NULL) {
        return '?';
    }
    if (p[1] == ':') {
        optind++;
        if (optind >= argc) {
            return '?';
        }
        optarg = argv[optind];
        optind++;
    }
    return opt;
}

#ifdef __cplusplus
}
#endif

